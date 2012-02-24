// File BitColumnExpression.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef BitColumnExpression_H
#define BitColumnExpression_H

#include "ColumnExpression.h"

namespace odb {
namespace sql {
namespace expression {

class BitColumnExpression : public ColumnExpression {
public:
	BitColumnExpression(const string&, const string&, SQLTable*);
	BitColumnExpression(const string&, const string&, const string&);
	~BitColumnExpression(); // Change to virtual if base class
private:
// No copy allowed
	BitColumnExpression(const BitColumnExpression&);
	BitColumnExpression& operator=(const BitColumnExpression&);

	unsigned long mask_;
	unsigned long shift_;

	string field_;
	string name_;

// -- Overridden methods
	virtual void prepare(SQLSelect& sql);
	virtual double eval(bool& missing) const;
	virtual void expandStars(const std::vector<SQLTable*>&,expression::Expressions&);
	virtual const odb::sql::type::SQLType* type() const;

	//friend ostream& operator<<(ostream& s,const BitColumnExpression& p)
	//	{ p.print(s); return s; }

};

} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
