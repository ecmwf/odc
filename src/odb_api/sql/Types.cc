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

namespace odc {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

odc::ColumnType sqlToOdbType(const SQLType& t) {

    switch(t.getKind()) {
    case SQLType::realType:
        return odc::REAL;
    case SQLType::integerType:
        return odc::INTEGER;
    case SQLType::stringType:
        return odc::STRING;
    case SQLType::bitmapType:
        return odc::BITFIELD;
    case SQLType::doubleType:
        return odc::DOUBLE;
    case SQLType::blobType:
        throw SeriousBug("SQL blob type not supported in odb_api", Here());
    default:
        throw SeriousBug("Unrecognised type found: " + Translator<int, std::string>()(t.getKind()), Here());
    };

    ASSERT(false);
    return odc::IGNORE;
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace odc
