/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/type/SQLType.h"
#include "eckit/utils/Translator.h"

#include "odb_api/sql/Types.h"

using namespace eckit;
using namespace eckit::sql::type;

namespace odb {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

odb::ColumnType sqlToOdbType(const SQLType& t) {

    switch(t.getKind()) {
    case SQLType::realType:
        return odb::REAL;
    case SQLType::integerType:
        return odb::INTEGER;
    case SQLType::stringType:
        return odb::STRING;
    case SQLType::bitmapType:
        return odb::BITFIELD;
    case SQLType::doubleType:
        return odb::DOUBLE;
    case SQLType::blobType:
        throw SeriousBug("SQL blob type not supported in odb_api", Here());
    default:
        throw SeriousBug("Unrecognised type found: " + Translator<int, std::string>()(t.getKind()), Here());
    };

    ASSERT(false);
    return odb::IGNORE;
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace odb
