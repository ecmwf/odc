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

#include "odc/sql/Types.h"

using namespace eckit;
using namespace eckit::sql::type;
using namespace odc::api;

namespace odc {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

ColumnType sqlToOdbType(const SQLType& t) {

    switch(t.getKind()) {
    case SQLType::realType:
        return REAL;
    case SQLType::integerType:
        return INTEGER;
    case SQLType::stringType:
        return STRING;
    case SQLType::bitmapType:
        return BITFIELD;
    case SQLType::doubleType:
        return DOUBLE;
    case SQLType::blobType:
        throw SeriousBug("SQL blob type not supported in odc", Here());
    default:
        throw SeriousBug("Unrecognised type found: " + Translator<int, std::string>()(t.getKind()), Here());
    };

    ASSERT(false);
    return IGNORE;
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace odc
