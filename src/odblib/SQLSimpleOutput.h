/// \file SQLSimpleOutput.h
/// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLSimpleOutput_H
#define SQLSimpleOutput_H

#include "SQLOutput.h"

namespace odb {
namespace sql {

class SQLSimpleOutput : public SQLOutput {
public:
	SQLSimpleOutput(ostream& = cout);
	virtual ~SQLSimpleOutput(); // Change to virtual if base class

protected:
	virtual void print(ostream&) const; // Change to virtual if base class	

private:
	SQLSimpleOutput(const SQLSimpleOutput&);
	SQLSimpleOutput& operator=(const SQLSimpleOutput&);

	ostream& out_;
	unsigned long long count_;

// -- Overridden methods
	virtual void size(int);
	virtual void reset();
	virtual void flush();
	virtual bool output(const expression::Expressions&);
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual unsigned long long count();

	virtual void outputReal(double, bool) const;
	virtual void outputDouble(double, bool) const;
	virtual void outputInt(double, bool) const;
	virtual void outputUnsignedInt(double, bool) const;
	virtual void outputString(double, bool) const;
	virtual void outputBitfield(double, bool) const;
};

} // namespace sql
} // namespace odb

#endif
