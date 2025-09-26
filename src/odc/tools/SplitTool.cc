/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "SplitTool.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/log/Log.h"
#include "eckit/types/Types.h"

#include "odc/DispatchingWriter.h"
#include "odc/LibOdc.h"
#include "odc/Reader.h"
#include "odc/Select.h"
#include "odc/TemplateParameters.h"
#include "odc/core/TablesReader.h"

using namespace eckit;
using namespace std;

namespace odc {
namespace tool {

SplitTool::SplitTool(int argc, char* argv[]) : Tool(argc, argv), maxOpenFiles_(200), sort_(false) {
    registerOptionWithArgument("-maxopenfiles");
}

void SplitTool::run() {
    if (parameters().size() != 3) {
        Log::error() << "Usage: ";
        usage(parameters(0), Log::error());
        Log::error() << endl;
        std::stringstream ss;
        ss << "Expected exactly 3 command line parameters";
        throw UserError(ss.str());
    }

    if (optionIsSet("-sort"))
        sort_ = true;

    maxOpenFiles_ = optionArgument("-maxopenfiles", maxOpenFiles_);
    LOG_DEBUG_LIB(LibOdc) << "SplitTool: maxOpenFiles_ = " << maxOpenFiles_ << endl;

    PathName inFile(parameters(1));
    string outFileTemplate(parameters(2));

    if (sort_)
        presortAndSplit(inFile, outFileTemplate);
    else
        split(inFile, outFileTemplate, maxOpenFiles_, !optionIsSet("-no_verification"));
}

/**
 * @param maxExpandedSize maximum size of the data in chunks after decoding
 */
vector<pair<Offset, Length> > SplitTool::getChunks(const PathName& inFile, size_t maxExpandedSize) {
    LOG_DEBUG_LIB(LibOdc) << "SplitTool::getChunks: " << endl;

    vector<pair<Offset, Length> > r;

    core::TablesReader reader(inFile);
    auto it(reader.begin()), end(reader.end());

    Offset currentOffset(0);
    Length currentLength(0);
    size_t currentSize(0);

    for (; it != end; ++it) {
        Offset offset(it->startPosition());
        Length length(it->nextPosition() - it->startPosition());
        size_t numberOfRows(it->rowCount());
        size_t numberOfColumns(it->columnCount());

        LOG_DEBUG_LIB(LibOdc) << "SplitTool::getChunks: " << offset << " " << length << endl;

        size_t size(numberOfRows * numberOfColumns * sizeof(double));
        if (currentSize + size > maxExpandedSize) {
            LOG_DEBUG_LIB(LibOdc) << "SplitTool::getChunks: collect " << currentOffset << " " << currentLength << endl;
            r.push_back(make_pair(currentOffset, currentLength));
            currentOffset = offset;
            currentLength = length;
        }
        else {
            currentLength += length;
            currentSize += numberOfRows * numberOfColumns * sizeof(double);
        }
    }
    if (r.size() == 0 || r.back().first != currentOffset)
        r.push_back(make_pair(currentOffset, currentLength));
    return r;
}

std::string SplitTool::genOrderBySelect(const std::string& inFile, const std::string& outFileTemplate) {
    core::TablesReader reader(inFile);
    auto it = reader.begin();
    TemplateParameters templateParameters;
    TemplateParameters::parse(outFileTemplate, templateParameters, it->columns());
    std::stringstream ss;
    ss << "select * order by ";
    for (size_t i = 0; i < templateParameters.size(); ++i) {
        if (i)
            ss << ",";
        ss << templateParameters[i]->name;
    }
    std::string sql(ss.str());
    Log::info() << "SplitTool::genOrderBySelect: sql: '" << sql << "'" << endl;
    return sql;
}

void SplitTool::presortAndSplit(const PathName& inFile, const std::string& outFileTemplate) {
    odc::DispatchingWriter out(outFileTemplate, 1);
    odc::DispatchingWriter::iterator outIt(out.begin());

    string sql(genOrderBySelect(inFile, outFileTemplate));

    vector<std::pair<Offset, Length> > chunks(getChunks(inFile));
    for (size_t i = 0; i < chunks.size(); ++i) {
        PartFileHandle h(inFile, chunks[i].first, chunks[i].second);
        h.openForRead();
        AutoClose closer(h);
        odc::Select in(sql, h);
        outIt->pass1(in.begin(), in.end());
    }
}

void SplitTool::split(const PathName& inFile, const std::string& outFileTemplate, size_t maxOpenFiles, bool verify) {
    odc::Reader in(inFile);
    odc::DispatchingWriter out(outFileTemplate, maxOpenFiles);

    odc::DispatchingWriter::iterator outIt(out.begin());
    outIt->pass1(in.begin(), in.end());

    odc::Reader input(inFile);
    odc::Reader::iterator begin(input.begin());
    odc::Reader::iterator end(input.end());
    outIt->close();
    if (verify)
        (**outIt).verify(begin, end);
}

}  // namespace tool
}  // namespace odc
