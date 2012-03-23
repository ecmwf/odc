/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/oda.h"

#include "odblib/ODAColumn.h"

namespace odb {
namespace sql {

ODAColumn::ODAColumn(const type::SQLType& type, SQLTable& owner, const string& name, int index, bool hasMissingValue, double
missingValue, bool isBitfield, const BitfieldDef& bitfieldDef, double* value)
: SQLColumn(type, owner, name, index, hasMissingValue, missingValue, isBitfield, bitfieldDef),
   value_(value)
{}

ODAColumn::~ODAColumn() {}

void ODAColumn::rewind() { *value_ = missingValue_; }

double ODAColumn::next(bool& missing)
{
	// FIXME: what if the column hasn't got a missing value????
	missing = (*value_ == missingValue_);
	return *value_;
}

void ODAColumn::advance(unsigned long) { NOTIMP; }

} // namespace sql 
} // namespace odb 

