#include "SQLDouble.h"
#include "SQLOutput.h"

namespace odb {
namespace sql {
namespace type {

SQLDouble::SQLDouble(const string& name):
	SQLType(name)
{}

SQLDouble::~SQLDouble() {}

size_t SQLDouble::size() const { return sizeof(double); }

void SQLDouble::output(SQLOutput& o, double d, bool m) const
{
	o.outputDouble(d, m);
}

static SQLDouble double_("double");

} // namespace type 
} // namespace sql
} // namespace odb
