/// \file FunctionROWNUMBER.h
/// Piotr Kuchta - (C) ECMWF July 2009

#include <utility>
#include <string>
#include <vector>

#include "SQLSession.h"
#include "SQLSelect.h"
#include "FunctionROWNUMBER.h"
#include "Log.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionROWNUMBER::FunctionROWNUMBER(const string& name, const expression::Expressions& args)
: FunctionExpression(name, args),
  count_(0)
{}


FunctionROWNUMBER::FunctionROWNUMBER(const FunctionROWNUMBER& other)
: FunctionExpression(other.name_, other.args_),
  count_(other.count_)
{}

SQLExpression* FunctionROWNUMBER::clone() const { return new FunctionROWNUMBER(*this); }

FunctionROWNUMBER::~FunctionROWNUMBER() {}

void FunctionROWNUMBER::print(ostream& s) const { s << "rownumber()"; }

double FunctionROWNUMBER::eval(bool& missing) const { return *count_; }

void FunctionROWNUMBER::prepare(SQLSelect& sql)
{
    SQLSelect &sqlSelect = dynamic_cast<SQLSelect &>(SQLSession::current().currentSQLStatement());
    count_ = &sqlSelect.total_;
}

void FunctionROWNUMBER::cleanup(SQLSelect& sql) {}

bool FunctionROWNUMBER::isConstant() const { return false; }

SQLExpression* FunctionROWNUMBER::simplify(bool&) { return 0; }

void FunctionROWNUMBER::partialResult() { /*NOTIMP;*/ }

const odb::sql::type::SQLType* FunctionROWNUMBER::type() const
{
    return &odb::sql::type::SQLType::lookup("integer");
}

void FunctionROWNUMBER::output(ostream& s) const
{
    bool missing;
    s << static_cast<unsigned long>(eval(missing));
}

static FunctionMaker<FunctionROWNUMBER> make_ROWNUMBER("rownumber", 0);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb
