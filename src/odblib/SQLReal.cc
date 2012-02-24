#include "SQLReal.h"
#include "SQLOutput.h"

namespace odb {
namespace sql {
namespace type {

SQLReal::SQLReal(const string& name):
	SQLType(name)
{}

SQLReal::~SQLReal() {}

size_t SQLReal::size() const { return sizeof(double); }

void SQLReal::output(SQLOutput& o, double d, bool m) const
{
	o.outputReal(d, m);
}

static SQLReal real("real");

} // namespace type 
} // namespace sql
} // namespace odb
