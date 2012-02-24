// File SQLStatement.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLStatement_H
#define SQLStatement_H

#include <machine.h>
#include "SQLExpression.h"

namespace odb {
namespace sql {
// Forward declarations

class SQLDatabase;

class SQLStatement {
public:
	SQLStatement();
	virtual ~SQLStatement(); // Change to virtual if base class

	virtual unsigned long long execute() = 0;
	virtual expression::Expressions output() const = 0;

protected:
	virtual void print(ostream&) const; // Change to virtual if base class	

private:
// No copy allowed
	SQLStatement(const SQLStatement&);
	SQLStatement& operator=(const SQLStatement&);


	friend ostream& operator<<(ostream& s,const SQLStatement& p)
		{ p.print(s); return s; }
};

} // namespace sql 
} // namespace odb 

#endif
