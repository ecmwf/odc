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
#include "odb_api/Partitioner.h"
#include "odb_api/RowsCounter.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

using namespace eckit;

namespace odc {

Partitions Partitioner::createPartitions(const std::vector<eckit::PathName>& files, size_t numberOfPartitions)
{
    vector<PathName> indices;
    for (size_t i (0); i < files.size(); ++i)
        indices.push_back(files[i] + ".idx");

    return createPartitions(files, indices, numberOfPartitions);
}

Partitions Partitioner::createPartitions(const std::vector<eckit::PathName>& files, const std::vector<eckit::PathName>& indices, size_t numberOfPartitions)
{
    if (numberOfPartitions > 2) --numberOfPartitions;

    Partitions parts;
    ullong totalRowsNumber (countRows (files, indices));
    ullong rowsPerPartition ((totalRowsNumber / numberOfPartitions));

    Log::info() << "*** createPartitions: numberOfPartitions: " << numberOfPartitions << ", totalRowsNumber: " << totalRowsNumber << ", rowsPerPartition: " << rowsPerPartition <<  endl;
    for (size_t i(0); i < indices.size(); ++i)
    {
        odc::Select in("select block_begin, block_length, seqno, n_rows;", indices[i]); 
        for (odc::Select::iterator it (in.begin()), end (in.end()); it != end; ++it)
        {
            const ullong blockStart ((*it)[0]),
                         blockLength ((*it)[1]),
                         seqno ((*it)[2]),
                         nRows ((*it)[3]);

            parts.addReport(files[i], blockStart, blockLength, seqno, nRows, rowsPerPartition);

/*
            if ( parts.back().numberOfRows() + nRows <= rowsPerPartition)
                parts.back().add(files[i], blockStart, blockLength, seqno, 0, nRows);
            else
            {
                const Block& last (parts.back().blocks().back());
                ullong firstRow ( newFile ? 0
                                  :  last.blockStart + last.blockLength == Offset(blockStart) + Length(blockLength) 
                                     ? parts.back().rowsOnLastBlock()
                                     : 0 );

                parts.push_back(Partition());
                parts.back().add(files[i], blockStart, blockLength, seqno, firstRow, nRows);
            }
            newFile = false;
*/
        }
    }
    return parts;
}

ullong Partitioner::countRows(const std::vector<eckit::PathName>& files, const std::vector<eckit::PathName>& indices)
{
    ullong n (0);
    for (size_t i(0); i < files.size(); ++i)
        n += RowsCounter::fastRowCount(files[i]);
    return n;
}

} // namespace odc 

