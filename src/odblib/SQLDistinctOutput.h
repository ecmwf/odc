// File SQLDistinctOutput..h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLDistinctOutput_H
#define SQLDistinctOutput_H

#include "SQLOutput.h"

namespace odb {
namespace sql {
namespace expression {

class SQLDistinctOutput : public SQLOutput {
public:
	SQLDistinctOutput(SQLOutput* output);
	virtual ~SQLDistinctOutput(); // Change to virtual if base class

protected:
	virtual void print(ostream&) const; // Change to virtual if base class	
private:
// No copy allowed
	SQLDistinctOutput(const SQLDistinctOutput&);
	SQLDistinctOutput& operator=(const SQLDistinctOutput&);

	virtual const SQLOutputConfig& config();
	virtual	void config(SQLOutputConfig&);
// -- Members
	auto_ptr<SQLOutput>   output_;
	set<vector<double> >  seen_;
	vector<double>        tmp_;
// -- Overridden methods
	virtual void size(int);
	virtual void reset();
	virtual void flush();
	virtual bool output(const expression::Expressions&);
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual unsigned long long count();

	virtual void outputReal(double, bool) const { NOTIMP; };
	virtual void outputDouble(double, bool) const { NOTIMP; };
	virtual void outputInt(double, bool) const { NOTIMP; };
	virtual void outputUnsignedInt(double, bool) const { NOTIMP; };
	virtual void outputString(double, bool) const { NOTIMP; };
	virtual void outputBitfield(double, bool) const { NOTIMP; };
};

} // namespace expression 
} // namespace sql 
} // namespace odb 

#endif
