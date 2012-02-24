#include "oda.h"

#include "ODAColumn.h"

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

