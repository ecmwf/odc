#include "SQLBitColumn.h"
#include "Log.h"
#include "SQLType.h"
#include "Exceptions.h"
#include "SQLBitfield.h"

namespace odb {
namespace sql {

SQLBitColumn::SQLBitColumn(SQLColumn& column,const string& field):
	SQLColumn(column),
	field_(field),
	mask_(0),
	shift_(0)
{
	const type::SQLBitfield& t = dynamic_cast<const type::SQLBitfield&>(type());
	mask_  = t.mask(field);
	shift_ = t.shift(field);
	
	Log::info() << "here " << field << " mask=" << hex << mask_ << dec << " shift=" << shift_ << endl;
}

SQLBitColumn::~SQLBitColumn() {}

void SQLBitColumn::rewind() { SQLColumn::rewind(); }

double SQLBitColumn::next(bool& missing)
{
	Log::info() << "SQLBitColumn::next: " << endl;

	unsigned long value = static_cast<unsigned long>(SQLColumn::next(missing));
	return (value >> shift_) & mask_;
}

void SQLBitColumn::advance(unsigned long n) { SQLColumn::advance(n); }

void SQLBitColumn::print(ostream& s) const
{
}

} // namespace sql
} // namespace odb
