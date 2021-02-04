/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/Indexer.h"

#include "eckit/eckit.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/PartFileHandle.h"

#include "odc/core/TablesReader.h"
#include "odc/core/MetaData.h"
#include "odc/Reader.h"
#include "odc/RowsCounter.h"
#include "odc/Select.h"
#include "odc/SelectIterator.h"
#include "odc/Writer.h"

using namespace eckit;
using namespace odc::core;

namespace odc {

BlockOffsets Indexer::offsetsOfBlocks(const PathName &db)
{
    BlockOffsets r;

    core::TablesReader reader(db);
    auto it = reader.begin();
    auto end = reader.end();
	for (; it != end; ++it)
    {
        Offset offset = it->startPosition();
        Length length = it->nextPosition() - it->startPosition();

        r.push_back(std::make_pair(offset,length));
    }

	return r;
}

std::vector<eckit::PathName> Indexer::createIndex(const std::vector<PathName> &dataFiles)
{
    std::vector<eckit::PathName> indices;
    for (size_t i(0); i < dataFiles.size(); ++i)
    {
        const PathName index (dataFiles[i] + ".idx");

        createIndex(dataFiles[i], index);
        indices.push_back(index);
    }
    return indices;
}

void Indexer::createIndex(const PathName &dataFile, const PathName& indexFile)
{
    BlockOffsets offsets (offsetsOfBlocks(dataFile));

    MetaData metaData;
    metaData
        .addColumn("block_begin", "INTEGER")
        .addColumn("block_length", "INTEGER")
        .addColumn("seqno", "INTEGER")
        .addColumn("n_rows", "INTEGER");

    odc::Writer<> write (indexFile);
    odc::Writer<>::iterator writer (write.begin());
    writer->columns(metaData);
    writer->writeHeader();

    for (size_t i(0); i < offsets.size(); ++i)
    {
        Offset blockBegin (offsets[i].first);
        Length blockLength (offsets[i].second);

        PartFileHandle h(dataFile, blockBegin, blockLength);
        h.openForRead();
        AutoClose closer(h);

        int prevSeqno (-1);
        int nRows (0);

        odc::Select in("select seqno;", h); 
        for (odc::Select::iterator it (in.begin()), end (in.end()); 
             it != end; 
             ++it)
        {
            int seqno ( (*it)[0] );
            if (seqno == prevSeqno)
                ++nRows;
            else
            {
                if (nRows > 0)
                {
                    (*writer)[0] = blockBegin;
                    (*writer)[1] = blockLength;
                    (*writer)[2] = prevSeqno;
                    (*writer)[3] = nRows;
                    ++writer;
                    nRows = 0;
                }
                prevSeqno = seqno;
                ++nRows;
            }
        }
        if (nRows > 0)
        {
            (*writer)[0] = blockBegin;
            (*writer)[1] = blockLength;
            (*writer)[2] = prevSeqno;
            (*writer)[3] = nRows;
            ++writer;
            nRows = 0;
        }
    }
}

} // namespace odc 

