// File SQLOutput.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLOutput_H
#define SQLOutput_H

#include "Exceptions.h"
#include "SQLExpression.h"
#include "SQLOutputConfig.h"

namespace odb {
namespace sql {

class SQLOutput {
public:
	SQLOutput();
	virtual ~SQLOutput(); // Change to virtual if base class

	virtual void size(int) = 0;

	virtual void prepare(SQLSelect&) = 0;
	virtual void cleanup(SQLSelect&) = 0;

	virtual void reset() = 0;
	virtual void flush() = 0;

	virtual bool output(const expression::Expressions&) = 0;

	virtual void outputReal(double, bool) const = 0;
	virtual void outputDouble(double, bool) const = 0;
	virtual void outputInt(double, bool) const = 0;
	virtual void outputUnsignedInt(double, bool) const = 0;
	virtual void outputString(double, bool) const = 0;
	virtual void outputBitfield(double, bool) const = 0;

	virtual const SQLOutputConfig& config();
	virtual	void config(SQLOutputConfig&);

	virtual unsigned long long count() = 0;

protected:
	SQLOutputConfig config_;

	virtual void print(ostream&) const; // Change to virtual if base class	

private:
// No copy allowed
	SQLOutput(const SQLOutput&);
	SQLOutput& operator=(const SQLOutput&);
// -- Friends
	friend ostream& operator<<(ostream& s,const SQLOutput& p)
		{ p.print(s); return s; }

};

} // namespace sql
} // namespace odb

#endif
