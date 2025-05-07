/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date January 2019

#ifndef odc_core_exceptions_H
#define odc_core_exceptions_H

#include "eckit/exception/Exceptions.h"


namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------


class ODBDecodeError : public eckit::Exception {
public:

    ODBDecodeError(const std::string&, const eckit::CodeLocation&);
};


class ODBIncomplete : public ODBDecodeError {
public:

    ODBIncomplete(const std::string&, const eckit::CodeLocation&);
};


class ODBInvalid : public ODBDecodeError {
public:

    ODBInvalid(const std::string& title, const std::string& reason, const eckit::CodeLocation&);
};


class ODBEndOfDataStream : public ODBDecodeError {
public:

    ODBEndOfDataStream(const std::string&, const eckit::CodeLocation&);
};


/// Exception thrown when a column without an associated table name is found in multiple tables.
class AmbiguousColumnException : public eckit::UserError {
public:

    explicit AmbiguousColumnException(const std::string& columnName);
};

/// Exception thrown when a requested column is not found.
class ColumnNotFoundException : public eckit::UserError {
public:

    explicit ColumnNotFoundException(const std::string& columnName);
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc

#endif
