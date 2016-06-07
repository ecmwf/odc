/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"
#include "odb_api/MetaData.h"
#include "odb_api/MetaDataReader.h"
#include "odb_api/MetaDataReaderIterator.h"
#include "odb_api/Reader.h"
#include "odb_api/Select.h"
#include "odb_api/Partitions.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"
#include "eckit/filesystem/PathName.h"

using namespace eckit;
using namespace std;

namespace odb {

std::ostream& operator<< (std::ostream& o, const Partitions& p)
{
    for (size_t i (0); i < p.size(); ++i)
        p[i].save(o, i);
        //o << "[" << i << ":" << i << p[i] <<  "], ";
    return o;
}

vector<PathName> Partitions::write(const PathName& fileNamePrefix) const
{
    vector<PathName> r;

    for (size_t i(0); i < size(); ++i)
    {
        const Partition& p (at(i));

        stringstream ss;
        ss << fileNamePrefix << ".pool_" << i;
        PathName partitionFileName (ss.str());

        Log::info() << "" << " ##### Writing partition " << i << " to file " << partitionFileName << endl;

        ullong n (p.write(partitionFileName));

        r.push_back(partitionFileName);
    }
    return r;
}

void Partitions::save(const PathName& partitionFile)
{
    Log::info() << "Saving partitions info to " << partitionFile << endl;

    ofstream f;
    f.exceptions(ofstream::badbit | ofstream::failbit);
    f.open(string(partitionFile).c_str());
    f << *this;
    f.close();
}

std::string Partitions::str() const
{
    stringstream ss;
    ss << *this;
    return ss.str();
}

void Partitions::addReport(const PathName& fileName, ullong blockStart, ullong blockLength, ullong seqno, ullong nRows, ullong rowsPerPartition)
{
    Partitions& parts (*this);
    if (parts.empty()) 
        parts.push_back(Partition());

    Partition& currentPartition (parts.back());
    // Do we have to open new partition?
    if (! (currentPartition.numberOfRows() + nRows <= rowsPerPartition)) {
        // Open new partition
        ullong firstRow (0);
        if (! currentPartition.blocks().empty()) {
            const Block& lastBlock (currentPartition.blocks().back());
            if (lastBlock.fileName == fileName && currentPartition.startOfLastBlock() == Offset(blockStart))
                firstRow = currentPartition.rowsOnLastBlock();
        }

        parts.push_back(Partition());
        Partition& newPartition (parts.back());

        newPartition.add(Block(fileName, blockStart, blockStart + blockLength, firstRow, firstRow + nRows));
        newPartition.startOfLastBlock(Offset(blockStart));
        newPartition.rowsOnLastBlock(nRows);
    } else {
        // Add to existing partition.
        // First block seen?
        if (currentPartition.blocks().empty()) {
            currentPartition.add(Block(fileName, blockStart, blockStart + blockLength, /*firstRow*/ 0, /*lastRow*/ nRows));
            currentPartition.startOfLastBlock(Offset(blockStart));
            currentPartition.rowsOnLastBlock(/*firstRow*/ 0 + nRows);
        } else {
            // If this is a new file then we need to create a new block
            Block& currentBlock (currentPartition.blocks().back());

            if (currentBlock.fileName != fileName) {
                currentPartition.add(Block(fileName, blockStart, blockStart + blockLength, /*firstRow*/ 0, /*lastRow*/ nRows));
                currentPartition.startOfLastBlock(Offset(blockStart));
                currentPartition.rowsOnLastBlock(/*firstRow*/ 0 + nRows);
            } else {
                // It's a block on the same file as the previously processed report.
                // If this is a report in a new physical block then we need to adjust current block's boundaries
                if (currentBlock.end < Offset(blockStart + blockLength))
                    currentBlock.end = blockStart + blockLength;

                currentBlock.lastRow += nRows;

                if (currentPartition.startOfLastBlock() == Offset(blockStart))
                    currentPartition.rowsOnLastBlock(currentPartition.rowsOnLastBlock() + nRows);
                else {
                    currentPartition.startOfLastBlock(Offset(blockStart));
                    currentPartition.rowsOnLastBlock(/*firstRow*/ 0 + nRows);
                }
            }
        }
    }
}

} // namespace odb 

