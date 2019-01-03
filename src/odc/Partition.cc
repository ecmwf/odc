/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/Partition.h"

#include <fstream>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/parser/StringTools.h"

#include "odc/MetaData.h"
#include "odc/Reader.h"
#include "odc/Select.h"
#include "odc/WriterBufferingIterator.h"
#include "odc/Writer.h"

using namespace eckit;
using namespace std;

namespace odc {

std::ostream& operator<< (std::ostream& o, const Partition& p)
{
    o << " number of blocks: " << p.blocks_.size() << " ";

    for (size_t i (0); i < p.blocks_.size(); ++i)
        o << p.blocks_[i] << ", ";
    return o;
}

std::ostream& Partition::save(std::ostream& o, size_t poolNumber) const
{
    for (size_t i (0); i < blocks_.size(); ++i)
        o << poolNumber << "\t" << blocks_[i] << endl;
    return o;
}

Partition::Partition(const PathName& fileName, size_t partitionNumber)
{
    Log::info() << "Partition::Partition: read info on partition " << partitionNumber << " from " << fileName << endl;
    if (! fileName.exists())
        throw UserError(string(fileName) + " does not exist");

    std::ifstream f (string(fileName).c_str());

    string line;
    while (std::getline(f, line))
    {
        Log::info() << "Partition::Partition: line: " << line << endl;

        vector<string> fs (StringTools::split("\t", line));
        ASSERT(fs.size() == 2);

        size_t part (atol(fs[0].c_str()));

        // TODO: Check that this has been updated correctly on the removal of ecml ExecutionContexts

        if (part == partitionNumber)
            add( Block (eckit::PathName(fs[1]),
                        eckit::Offset(atol(fs[2].c_str())),
                        eckit::Offset(atol(fs[3].c_str())),
                        atol(fs[4].c_str()),
                        atol(fs[5].c_str())));
    }
}

Partition::Partition()
: blocks_ (),
  startOfLastBlock_(0),
  rowsOnLastBlock_(0)
{}

Partition::Partition(const Partition& other)
: blocks_ (other.blocks_),
  startOfLastBlock_ (other.startOfLastBlock_),
  rowsOnLastBlock_ (other.rowsOnLastBlock_)
{}

Partition& Partition::operator=(const Partition& other)
{
    blocks_ = other.blocks_;
    startOfLastBlock_ = other.startOfLastBlock_;
    rowsOnLastBlock_ = other.rowsOnLastBlock_;
    return *this;
}

ullong Partition::numberOfRows() const
{
    ullong r (0);
    for (size_t i(0); i < blocks_.size(); ++i)
        r += blocks_[i].lastRow - blocks_[i].firstRow;
    return r;
}

ullong Partition::numberOfRowsOnLastBlock() const
{
    ASSERT(blocks_.size());
    return blocks_.back().lastRow;
}

void Partition::add(const Block& block)
{
    blocks_.push_back(block);
}

/*
void Partition::add(const PathName& fileName, ullong start, ullong length, ullong seqno, ullong firstRow, ullong nRows)
{
    if (! blocks_.size())
    {
        startOfLastBlock_ = Offset(start);
        rowsOnLastBlock_ = firstRow + nRows;
        blocks_.push_back(Block(fileName, start, length, firstRow, firstRow + nRows));
    }
    else
    {
        ASSERT(firstRow == 0); // new file, should start from beginning

        Block& last (blocks_.back());
        if (last.fileName != fileName)
            blocks_.push_back(Block(fileName, start, length, firstRow, firstRow + nRows));
        else
        {
            last.lastRow += nRows;

            if (startOfLastBlock_ == Offset(start))
                rowsOnLastBlock_ += nRows;
            else
            {
                startOfLastBlock_ = Offset(start);
                rowsOnLastBlock_ = firstRow + nRows;
                last.length += Length(length);
            }
        }
    }
}
*/


ullong writeBlock(DataHandle& in, const Block& block, Writer<>::iterator& out)
{
    odc::Reader reader(in);
    odc::Reader::iterator it (reader.begin()), end (reader.end());

    ullong rowNumber (0);
    for (size_t r(0); r < block.lastRow; ++r, ++it)
    {
        ASSERT(it != end);
        if (r >= block.firstRow)
        {
            if (out->columns() != it->columns())
            {
                out->columns(it->columns());
                out->writeHeader();
            }

            for (size_t fi (0); fi < it->columns().size(); ++fi)
                (*out)[fi] = (*it)[fi];

            ++out;
            ++rowNumber;
        }
    }
    return rowNumber;
}

ullong Partition::write(DataHandle& dh) const
{
    long long rowNumber (0);
    const vector<Block>& blocks (blocks_);
    for (size_t i (0); i < blocks.size(); ++i)
    {
        const Block& block (blocks[i]);
        Log::info() << "Partition::write: writing block " << i << ":" << block << endl;
        Writer<> writer(&dh, false, false);
        Writer<>::iterator out (writer.begin(/*openDataHandle*/ false));

        PartFileHandle fh (block.fileName, block.start, block.end - block.start);

        Log::info() << "Partition::write: writing PartFileHandle: " << fh << endl;

        fh.openForRead();
        ullong nr (writeBlock(fh, block, out));
        rowNumber += nr;
    }
    return rowNumber;

}

ullong Partition::write(const PathName& fileName) const
//{
//    FileHandle oh (fileName);
//    oh.openForWrite(Length(1024 * 1024 * 10));
//    return write(oh);
//}

//ullong Partition::write(DataHandle& dh) const
{
    long long rowNumber (0);
    const vector<Block>& blocks (blocks_);

    { PathName(fileName).unlink(); }

    for (size_t i (0); i < blocks.size(); ++i)
    {
        const Block& block (blocks[i]);

        Log::info() << "Partition::write: writing block " << i << ":" << block << endl;

        FileHandle dh (fileName);
        dh.openForAppend(Length(1024 * 1024 * 10));

        Writer<> writer(&dh, false, false);
        Writer<>::iterator out (writer.begin(/*openDataHandle*/ false));

        PartFileHandle fh (block.fileName, block.start, block.end - block.start);
        fh.openForRead();
        ullong nr (writeBlock(fh, block, out));
        rowNumber += nr;
    }
    return rowNumber;
}

} // namespace odc

