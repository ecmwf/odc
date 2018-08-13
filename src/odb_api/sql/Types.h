/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date Aug 2018

#ifndef odb_api_sql_Types_H
#define odb_api_sql_Types_H

#include "odb_api/ColumnType.h"

namespace eckit { namespace sql { namespace type { class SQLType; }}}

namespace odb {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

odb::ColumnType sqlToOdbType(const eckit::sql::type::SQLType& t);

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace odb

#endif
