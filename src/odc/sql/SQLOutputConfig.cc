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

#include "odc/DispatchingWriter.h"
#include "odc/sql/ODAOutput.h"
#include "odc/sql/SQLOutputConfig.h"
#include "odc/TemplateParameters.h"
#include "odc/Writer.h"

namespace odc {
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

eckit::sql::SQLOutput* SQLOutputConfig::buildOutput(const eckit::PathName& path) const {

    // TODO: maxOpenFiles configuration for output. Was disabled in Feb 2016
    const size_t maxOpenFiles = 100;

    std::string format (path.asString().empty() ? outputFormat_ : "odb");

    if (format == "default" || format == "wide") {
        return new eckit::sql::SQLSimpleOutput(*this, outStream_.get());
    } else if (format == "odb") {
        ASSERT(path.asString().size());
        TemplateParameters templateParameters;
        TemplateParameters::parse(path, templateParameters);
        if (templateParameters.size()) {
            return new odc::sql::ODAOutput<DispatchingWriter>(new DispatchingWriter(path, maxOpenFiles));
        } else {
            return new odc::sql::ODAOutput<Writer<>>(new Writer<>(path));
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
} // namespace odc
