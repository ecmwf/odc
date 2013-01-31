#include "odblib/SQLDataColumn.h"

namespace odb {

SQLDataColumn::SQLDataColumn(const odb::sql::type::SQLType& type,
        odb::sql::SQLTable& owner, const std::string& name, int index,
        bool hasMissingValue, double missingValue, bool isBitfield,
        const odb::BitfieldDef& bitfieldDef, double* value)
  : odb::sql::SQLColumn(type, owner, name, index, hasMissingValue, missingValue,
        isBitfield, bitfieldDef), value_(value)
{}

SQLDataColumn::~SQLDataColumn() {}

void SQLDataColumn::rewind() { *value_ = missingValue_; }

double SQLDataColumn::next(bool& missing)
{
    missing = (*value_ == missingValue_);
    return *value_;
}

void SQLDataColumn::advance(unsigned long) { NOTIMP; }

} // namespace odb
