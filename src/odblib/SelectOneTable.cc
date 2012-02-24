/// \file SelectOneTable.cc
/// Piotr Kuchta - ECMWF Jul 11

#include <machine.h>
#include <stack>

#include "SQLStatement.h"
#include "SQLColumn.h"
#include "SQLExpression.h"
#include "SQLOutput.h"
#include "FunctionROWNUMBER.h"
#include "SelectOneTable.h"

namespace odb {
namespace sql {

SelectOneTable::SelectOneTable(const SQLTable* table)
: table_(table),
  offset_(0),
  length_(0),
  column_(0),
  table1_(0),
  table2_(0),
  order_(0)
{}

SelectOneTable::~SelectOneTable() {}

} // namespace sql
} // namespace odb
