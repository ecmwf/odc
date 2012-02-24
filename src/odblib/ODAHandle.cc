#include "ODAHandle.h"
#include "ODATranslator.h"

ODAHandle::ODAHandle(Offset start, Offset end)
: start_(start),
  end_(end)
{
	Log::debug() << "ODAHandle::ODAHandle(" << start << ", " << end << ")" << endl; 
}

void ODAHandle::print(ostream& o) const
{
	o << "[start:" << start_<< ", end_:" << end_ << ", values_:" << values_ << "]";
}

ODAHandle::~ODAHandle()
{
	Log::debug() << "ODAHandle::~ODAHandle()" << endl;
}

void ODAHandle::addValue(const string& columnName, double v)
{
	Log::debug() << "ODAHandle::addValue('" << columnName << "', '" << v << "')" << endl;
	ASSERT(values_.find(columnName) == values_.end());
	values_[columnName] = v;
}

