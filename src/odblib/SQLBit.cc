#include "SQLBit.h"
#include "Exceptions.h"
#include "SQLOutput.h"

namespace odb {
namespace sql {
namespace type {

SQLBit::SQLBit(const string& name, unsigned long mask, unsigned long shift):
	type::SQLType(name),
	mask_(mask),
	shift_(shift)
{}

SQLBit::~SQLBit() {}

size_t SQLBit::size() const
{
	NOTIMP;
	// This should not be calles
	return sizeof(long);
}

void SQLBit::output(SQLOutput& o, double x, bool missing) const
{
	//Log::info() << "SQLBit::output: x=" << x << ", missing=" << missing << endl;
	//s << ((m & mask_) >> shift_);
	// TODO: does it work like this? test!
	o.outputUnsignedInt(x, missing);
}

} // namespace type 
} // namespace sql 
} // namespace odb 
