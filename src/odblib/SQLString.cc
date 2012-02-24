#include "SQLString.h"
#include "SQLOutput.h"

#include <ctype.h>

namespace odb {
namespace sql {
namespace type {

SQLString::SQLString(const string& name):
	SQLType(name)
{} 

SQLString::~SQLString() {}

size_t SQLString::size() const { return sizeof(double); }

void SQLString::output(SQLOutput& o, double d, bool missing) const
{
	o.outputString(d, missing);
}

static SQLString type("string");

} // namespace type 
} // namespace sql
} // namespace odb
