/// @file   DataSelectOutput.h
/// @author Tomas Kral

#ifndef DataSelectOutput_H
#define DataSelectOutput_H

#include "odb_api/odblib/SQLOutput.h"

namespace odb {
namespace internal {

class DataSelectIterator;

class DataSelectOutput
  : public odb::sql::SQLOutput
{
public:
	DataSelectOutput(DataSelectIterator& it);
   ~DataSelectOutput();

protected:
	virtual void print(std::ostream&) const {}

private:
	DataSelectOutput(const DataSelectOutput&);
	DataSelectOutput& operator=(const DataSelectOutput&);

    DataSelectIterator& it_;
    unsigned long long count_;

	virtual void size(int) {}
	virtual void reset() { count_ = 0; }
	virtual void flush() {}
	virtual bool output(const odb::sql::expression::Expressions&);
	virtual void prepare(odb::sql::SQLSelect&) {}
	virtual void cleanup(odb::sql::SQLSelect&) {}
	virtual unsigned long long count() { return count_; }

	virtual void outputReal(double, bool) const {}
	virtual void outputDouble(double, bool) const {}
	virtual void outputInt(double, bool) const {}
	virtual void outputUnsignedInt(double, bool) const {}
	virtual void outputString(double, bool) const {}
	virtual void outputBitfield(double, bool) const {}
};

} // namespace internal
} // namespace odb

#endif // DataSelectOutput_H
