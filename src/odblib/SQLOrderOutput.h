/// \file SQLOrderOutput.h
/// Piotr Kuchta - ECMWF Nov 11

#ifndef SQLOrderOutput_H
#define SQLOrderOutput_H

#include "SQLOutput.h"
#include "SQLExpressionEvaluated.h"
#include "OrderByExpressions.h"

namespace odb {
namespace sql {

class SQLOrderOutput : public SQLOutput {
public:
	SQLOrderOutput(SQLOutput* output, const pair<Expressions,vector<bool> >& by);
	virtual ~SQLOrderOutput();

protected:
	virtual void print(ostream&) const;

private:
// No copy allowed
	SQLOrderOutput(const SQLOrderOutput&);
	SQLOrderOutput& operator=(const SQLOrderOutput&);

// -- Members
	auto_ptr<SQLOutput> output_;
	pair<Expressions,vector<bool> > by_;
	
	typedef map<OrderByExpressions, vector<Expressions> >
			SortedResults;

	SortedResults sortedResults_;

// -- Overridden methods
	virtual void size(int);
	virtual void reset();
	virtual void flush();
	virtual bool output(const Expressions&);
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual unsigned long long count();

	virtual void outputReal(double, bool) const { NOTIMP; };
	virtual void outputDouble(double, bool) const { NOTIMP; };
	virtual void outputInt(double, bool) const { NOTIMP; };
	virtual void outputUnsignedInt(double, bool) const { NOTIMP; };
	virtual void outputString(double, bool) const { NOTIMP; };
	virtual void outputBitfield(double, bool) const { NOTIMP; };

	virtual const SQLOutputConfig& config();
	virtual void config(SQLOutputConfig&);

	friend ostream& operator<<(ostream& s, const SQLOrderOutput& o)
		{ o.print(s); return s; }
};

} // namespace sql
} // namespace odb

#endif
