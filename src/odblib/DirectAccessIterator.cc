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
/// \file DirectAccessIterator.cc
///
/// @author Baudouin Raoult, Dec 2013

#include <arpa/inet.h>


//#include "odblib/odb_api.h"
//#include "odblib/Codec.h"
//#include "odblib/DataStream.h"
//#include "odblib/Header.h"
#include "odblib/DirectAccessIterator.h"
//#include "odblib/DirectAccess.h"
//#include "odblib/MetaDataReaderIterator.h"

using namespace eckit;

namespace odb {

DirectAccessIterator::DirectAccessIterator(DirectAccess &owner)
    : owner_(owner),
      columns_(0),
      lastValues_(0),
      codecs_(0),
      nrows_(0),
      f(0),
      newDataset_(false),
      noMore_(false),
      ownsF_(false),
      headerCounter_(0),
      byteOrder_(BYTE_ORDER_INDICATOR),
      refCount_(0),
      lastOffset_(0)
{
    f = owner.dataHandle();
    ASSERT(f);

    initBlocks();
}

DirectAccessIterator::DirectAccessIterator(DirectAccess &owner, const PathName& pathName)
    : owner_(owner),
      columns_(0),
      lastValues_(0),
      codecs_(0),
      nrows_(0),
      f(0),
      newDataset_(false),
      noMore_(false),
      ownsF_(false),
      headerCounter_(0),
      byteOrder_(BYTE_ORDER_INDICATOR),
      refCount_(0),
      lastOffset_(0)
{
    f = new FileHandle(pathName);
    ASSERT(f);
    ownsF_ = true;
    f->openForRead();

    initBlocks();
}

DirectAccessIterator::~DirectAccessIterator ()
{
    Log::debug() << "DirectAccessIterator::~DirectAccessIterator: headers read: " << headerCounter_ << " rows:" << nrows_ << std::endl;

    close();
    delete [] lastValues_;
    delete [] codecs_;
}

void DirectAccessIterator::initBlocks()
{
    eckit::Timer timer("DirectAccessIterator::initBlocks");
    DataHandle* h = f->clone();
    h->openForRead();
    IteratorProxy<MetaDataReaderIterator, DirectAccessIterator, const double> it(new MetaDataReaderIterator(h, true));
    IteratorProxy<MetaDataReaderIterator, DirectAccessIterator, const double> end(0);

    unsigned long long n = 0;
    for (; it != end; ++it)
    {
        MetaData &md = it->columns();
        n += md.rowsNumber();
        //it.iter_->blockEndOffset();
        //std::cout << it.iter_->blockStartOffset() << std::endl;
    }

    std::cout << "Rows " << n << endl;

}


bool DirectAccessIterator::operator!=(const DirectAccessIterator& other)
{
    ASSERT(&other == 0);
    return noMore_;
}

void DirectAccessIterator::initRowBuffer()
{
    size_t nCols = columns().size();

    delete [] lastValues_;
    lastValues_ = new double [nCols];

    delete [] codecs_;
    codecs_ = new odb::codec::Codec* [nCols];

    for(size_t i = 0; i < nCols; i++)
    {
        codecs_[i] = &columns()[i]->coder();
        lastValues_[i] = codecs_[i]->missingValue();
        codecs_[i]->dataHandle(f);
    }
}


bool DirectAccessIterator::next()
{
    newDataset_ = false;
    if (noMore_)
        return false;

    uint16_t c = 0;
    long bytesRead = 0;

    if ( (bytesRead = f->read(&c, 2)) <= 0)
        return ! (noMore_ = true);

    ASSERT(bytesRead == 2);

    if (c == ODA_MAGIC_NUMBER)
    {
        Offset o = (unsigned long long)f->position() - (unsigned long long)2;
        Length l = o - lastOffset_;

        blocks_.push_back(std::make_pair(o,l));
        lastOffset_ = o;

        DataStream<SameByteOrder> ds(f);

        unsigned char cc;
        ds.readUChar(cc); ASSERT(cc == 'O');
        ds.readUChar(cc); ASSERT(cc == 'D');
        ds.readUChar(cc); ASSERT(cc == 'A');

        Header<DirectAccessIterator> header(*this);
        header.loadAfterMagic();
        byteOrder_ = header.byteOrder();
        ++headerCounter_;
        initRowBuffer();
        newDataset_ = true;

    }
    c = ntohs(c);

    size_t nCols = columns().size();
    for(size_t i = c; i < nCols; i++)
        lastValues_[i] = codecs_[i]->decode();

    ++nrows_ ;
    return nCols;
}

bool DirectAccessIterator::isNewDataset() { return newDataset_; }

const double* DirectAccessIterator::data() { return lastValues_; }
double& DirectAccessIterator::data(size_t i)
{
    ASSERT(i >= 0 && i < columns().size());
    return lastValues_[i];
}

int DirectAccessIterator::close()
{
    if (ownsF_ && f)
    {
        f->close();
        delete f;
        f = 0;
    }

    std::cout << "BLOCKS " << blocks_.size() << std::endl;

    return 0;
}


void DirectAccessIterator::property(std::string key, std::string value)
{
    properties_[key] = value;
}

std::string DirectAccessIterator::property(std::string key)
{
    return properties_[key];
}


ColumnType DirectAccessIterator::columnType(unsigned long index) { return columns_[index]->type(); }
const std::string& DirectAccessIterator::columnName(unsigned long index) const { return columns_[index]->name(); }
const std::string& DirectAccessIterator::codecName(unsigned long index) const { return columns_[index]->coder().name(); }
double DirectAccessIterator::columnMissingValue(unsigned long index) { return columns_[index]->missingValue(); }
const BitfieldDef& DirectAccessIterator::bitfieldDef(unsigned long index) { return columns_[index]->bitfieldDef(); }

} // namespace odb

