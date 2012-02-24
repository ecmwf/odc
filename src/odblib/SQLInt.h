// File SQLInt.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLInt_H
#define SQLInt_H

#include "SQLType.h"

namespace odb {
namespace sql {

class SQLOutput;

namespace type {

class SQLInt : public SQLType {
public:
	SQLInt(const string& );
	~SQLInt(); // Change to virtual if base class

private:
// No copy allowed
	SQLInt(const SQLInt&);
	SQLInt& operator=(const SQLInt&);

	virtual size_t size() const;
	virtual void output(SQLOutput& s, double, bool) const;
	virtual int getKind() const { return integerType; }

	//friend ostream& operator<<(ostream& s,const SQLInt& p)
	//	{ p.print(s); return s; }

};

} // namespace type 
} // namespace sql
} // namespace odb

#endif
