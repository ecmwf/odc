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
#include "odb_api/Indexer.h"
#include "odb_api/RowsCounter.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

using namespace eckit;

namespace odb {

BlockOffsets Indexer::offsetsOfBlocks(const PathName &db)
{
	typedef MetaDataReader<MetaDataReaderIterator> MDR;

    BlockOffsets r;

	MDR mdReader(db);
	MDR::iterator it (mdReader.begin());
	MDR::iterator end (mdReader.end());
	for (; it != end; ++it)
    {
        Offset offset ((**it).blockStartOffset());
        Length length ((**it).blockEndOffset() - (**it).blockStartOffset());

        r.push_back(make_pair(offset,length));
    }

	return r;
}

std::vector<eckit::PathName> Indexer::createIndex(const vector<PathName> &dataFiles)
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

    odb::MetaData metaData;
    metaData
        .addColumn("block_begin", "INTEGER")
        .addColumn("block_length", "INTEGER")
        .addColumn("seqno", "INTEGER")
        .addColumn("n_rows", "INTEGER");

    odb::Writer<> write (indexFile);
    odb::Writer<>::iterator writer (write.begin());
    writer->columns(metaData);
    writer->writeHeader();

    for (size_t i(0); i < offsets.size(); ++i)
    {
        Offset blockBegin (offsets[i].first);
        Length blockLength (offsets[i].second);

        PartFileHandle h(dataFile, blockBegin, blockLength);
        h.openForRead();

        int prevSeqno (-1);
        int nRows (0);

        odb::Select in("select seqno;", h); 
        for (odb::Select::iterator it (in.begin()), end (in.end()); 
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

} // namespace odb 

