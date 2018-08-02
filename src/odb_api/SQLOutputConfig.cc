/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odb_api/SQLOutputConfig.h"

namespace odb {

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
    outStream_(eckit::Log::info()) {}

SQLOutputConfig::~SQLOutputConfig() {}

eckit::sql::SQLOutput* SQLOutputConfig::buildOutput() const {
    NOTIMP;
}

void SQLOutputConfig::setOutputStream(std::ostream& s) {
    outStream_ = s;
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace odb
