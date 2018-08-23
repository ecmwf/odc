/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLSimpleOutput.h"

#include "odb_api/DispatchingWriter.h"
#include "odb_api/sql/ODAOutput.h"
#include "odb_api/sql/SQLOutputConfig.h"
#include "odb_api/TemplateParameters.h"
#include "odb_api/Writer.h"

namespace odb {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------


SQLOutputConfig::SQLOutputConfig(bool noColumnNames,
                                 bool noNULL,
                                 const std::string& delimiter,
                                 const std::string& format,
                                 bool bitfieldsBinary,
                                 bool noColumnAlignment,
                                 bool fullPrecision) :
    eckit::sql::SQLOutputConfig(noColumnNames, noNULL, delimiter, format,
                                bitfieldsBinary, noColumnAlignment, fullPrecision),
    outStream_(std::cout) {}

SQLOutputConfig::SQLOutputConfig(const std::string& odbFilename) :
    SQLOutputConfig() {
    outputFormat_ = "odb";
    outputFile_ = odbFilename;
}

SQLOutputConfig::~SQLOutputConfig() {}

eckit::sql::SQLOutput* SQLOutputConfig::buildOutput() const {

    // TODO: maxOpenFiles configuration for output. Was disabled in Feb 2016
    const size_t maxOpenFiles = 100;

    if (outputFormat_ == "default" || outputFormat_ == "wide") {
        return new eckit::sql::SQLSimpleOutput(*this, outStream_.get());
    } else if (outputFormat_ == "odb") {
        ASSERT(outputFile_.asString().size());
        TemplateParameters templateParameters;
        TemplateParameters::parse(outputFile_, templateParameters);
        if (templateParameters.size()) {
            return new odb::sql::ODAOutput<DispatchingWriter>(new DispatchingWriter(outputFile_, maxOpenFiles));
        } else {
            return new odb::sql::ODAOutput<Writer<>>(new Writer<>(outputFile_));
            // TODO: toODAColumns
        }

    }
    NOTIMP;
}

void SQLOutputConfig::setOutputStream(std::ostream& s) {
    outStream_ = s;
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace odb
