/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file ODA.cc
///
/// @author Baudouin Raoult, Dec 2013

#include <time.h>
#include <sys/time.h>

#include "eckit/eckit.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/PartHandle.h"
#include "eckit/io/SharedHandle.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Timer.h"
#include "odblib/DirectAccess.h"
#include "odblib/MetaDataReaderIterator.h"
#include "odblib/Reader.h"

using namespace std;
using namespace eckit;

namespace odb {

DirectAccessBlock::~DirectAccessBlock()
{
    delete handle_;
    delete[] data_;
}

void DirectAccessBlock::unload() {
    delete handle_; handle_ = 0;
    delete[] data_; data_ = 0;
    unloads_ ++;
}


DirectAccess::DirectAccess(DataHandle &dh, size_t maxBlocksSize)
    : HandleHolder(dh),
      current_(new DirectAccessIterator(*this)),
      row_(current_),
      maxBlocksSize_(maxBlocksSize),
      usedBlocksSize_(0)
{
    initBlocks();
}

DirectAccess::DirectAccess(DataHandle *dh, size_t maxBlocksSize)
    : HandleHolder(dh),
      current_(new DirectAccessIterator(*this)),
      row_(current_),
      maxBlocksSize_(maxBlocksSize),
      usedBlocksSize_(0)
{
    initBlocks();
}

DirectAccess::DirectAccess(const std::string& path, size_t maxBlocksSize)
    : HandleHolder(new FileHandle(path)),
      path_(path),
      current_(new DirectAccessIterator(*this)),
      row_(current_),
      maxBlocksSize_(maxBlocksSize),
      usedBlocksSize_(0)
{
    handle().openForRead();
    initBlocks();
}

void DirectAccess::initBlocks()
{
    eckit::Timer timer("DirectAccessIterator::initBlocks");
    IteratorProxy<MetaDataReaderIterator, DirectAccessIterator, const double> it(new MetaDataReaderIterator(handle(), true));
    IteratorProxy<MetaDataReaderIterator, DirectAccessIterator, const double> end(0);

    unsigned long long n = 0;
    size_t c = 0;
    for (; it != end; ++it)
    {
        MetaData &md = it->columns();
        n += md.rowsNumber();
        blocks_.push_back(DirectAccessBlock(c++,
                                            md.rowsNumber(),
                                            it.iter_->blockStartOffset(),
                                            it.iter_->blockEndOffset() - it.iter_->blockStartOffset()
                                            ));
    }

    std::cout << "Rows " << n << endl;
    eckit::Timer t("DirectAccessIterator::initBlocks (index)");

    ASSERT(size_t(n) == n);
    index_.reserve(n);

    for(std::deque<DirectAccessBlock>::iterator j = blocks_.begin(); j != blocks_.end(); ++j) {
        DirectAccessBlock& b = *j;
        size_t n = 0;
        for(size_t i = 0; i < b.rows(); ++i) {
            index_.push_back(std::make_pair(&b, n));
            n++;
        }
    }

}

DirectAccess::row* DirectAccess::operator[](size_t n)
{
    struct timeval t;

    ASSERT(n < index_.size());
    std::pair<DirectAccessBlock*, size_t>& e = index_[n];
    DirectAccessBlock* b = e.first;
    if(!b->handle()) {


        while(usedBlocksSize_ >= maxBlocksSize_) {
            // Unload blocks


            unsigned long long t = 0;
            std::deque<DirectAccessBlock>::iterator k;

            for(std::deque<DirectAccessBlock>::iterator j = blocks_.begin();
                j != blocks_.end(); ++j) {
                DirectAccessBlock& u = *j;
                if(u.handle()) {
                    if(t == 0 || u.last() < t) {
                        k = j;
                        t = u.last();
                    }
                }
            }

            ASSERT(t);
            usedBlocksSize_ -= (*k).size();
            (*k).unload();
            //std::cout << "UNLOAD " << (*k).n() << " maxBlocksSize " << maxBlocksSize_ << std::endl;

        }



        //std::cout << "LOADING block " << b->n() << " at offset " << eckit::Bytes(b->offset()) << ", length "
                  //<<  eckit::Bytes(b->length()) << std::endl;
        //std::cout << "INDEX is " << n << " offset in block is " << e.second << std::endl;
        b->handle(new PartHandle(new SharedHandle(handle()), b->offset(), b->length()));




    }

    if(!b->data())
    {


        Reader in(*b->handle());
        Reader::iterator it = in.begin();
        Reader::iterator end = in.end();
        MetaData& md = it->columns();
        //std::cout << "SIZE " << md.size() << std::endl;
        //std::cout << "ROWS " << md.rowsNumber() << std::endl;

        size_t width = md.size();
        size_t height = md.rowsNumber();

        b->size(width * height);
        usedBlocksSize_  += b->size();
        b->data(new double[b->size()]);
        b->metaData(md.clone());

        //eckit::Timer t("Read part");
        size_t n = 0;
        size_t off = 0;
        for(; it != end; ++it) {
            const double* d = it->data();
            std::copy(d, d+width, b->data() + off);
            n++;
            off += width;
        }
        ASSERT(n == height);

    }

    ::gettimeofday(&t,0);

    b->last(t.tv_sec * 1000000 + t.tv_usec) ;

    idx_   = e.second;
    block_ = b;
    return &(*current_);

}


DirectAccess::~DirectAccess()
{
    std::cout << "BLOCKS : " << blocks_.size() << std::endl;
    for(std::deque<DirectAccessBlock>::iterator j = blocks_.begin(); j != blocks_.end(); ++j) {
        DirectAccessBlock& b = *j;
        std::cout << "BLOCK " << b.n() << " loads: " << b.loads() << " unloads: " << b.unloads() << std::endl;
    }
}


} // namespace odb
