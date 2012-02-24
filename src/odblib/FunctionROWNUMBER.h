/// \file FunctionROWNUMBER.h
/// Piotr Kuchta - (C) ECMWF July 2009

#ifndef FunctionROWNUMBER_H
#define FunctionROWNUMBER_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionROWNUMBER : public FunctionExpression {
public:
	FunctionROWNUMBER(const string&, const expression::Expressions&);
	FunctionROWNUMBER(const FunctionROWNUMBER&);
	~FunctionROWNUMBER();

// -- Overridden methods
	virtual const odb::sql::type::SQLType* type() const;
	virtual void output(ostream& s) const;

	SQLExpression* clone() const;

protected:
// -- Overridden methods
	virtual void print(ostream& s) const;
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual bool isConstant() const;
	virtual void partialResult();
	virtual double eval(bool& missing) const;
	virtual SQLExpression* simplify(bool&);
	bool isAggregate() const { return false; }

private:
// No copy allowed
	FunctionROWNUMBER& operator=(const FunctionROWNUMBER&);

	unsigned long long* count_;
// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionROWNUMBER& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
