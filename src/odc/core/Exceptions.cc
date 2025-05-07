/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/Exceptions.h"


namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

ODBDecodeError::ODBDecodeError(const std::string& s, const eckit::CodeLocation& loc) :
    Exception(std::string("ODB decode failure: ") + s, loc) {}


ODBIncomplete::ODBIncomplete(const std::string& s, const eckit::CodeLocation& loc) :
    ODBDecodeError(std::string("Unexpected end of file: ") + s, loc) {}


ODBInvalid::ODBInvalid(const std::string& file, const std::string& reason, const eckit::CodeLocation&) :
    ODBDecodeError(std::string("Invalid ODB (") + file + ") -- " + reason, Here()) {}


ODBEndOfDataStream::ODBEndOfDataStream(const std::string& s, const eckit::CodeLocation& loc) : ODBDecodeError(s, loc) {}


AmbiguousColumnException::AmbiguousColumnException(const std::string& columnName) :
    UserError("Ambiguous column name: '" + columnName + "'") {}


ColumnNotFoundException::ColumnNotFoundException(const std::string& columnName) :
    UserError("Column '" + columnName + "' not found.") {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc
