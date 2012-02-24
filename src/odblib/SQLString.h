// File SQLString.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLString_H
#define SQLString_H

#include "SQLType.h"

namespace odb {
namespace sql {

class SQLOutput;

namespace type {

class SQLString : public SQLType {
public:
	SQLString(const string& );
	~SQLString(); // Change to virtual if base class

private:
// No copy allowed
	SQLString(const SQLString&);
	SQLString& operator=(const SQLString&);

// -- Overridden methods
	virtual size_t size() const;
	virtual void output(SQLOutput&, double, bool) const;
	virtual int getKind() const { return stringType; }

	//friend ostream& operator<<(ostream& s,const SQLString& p)
	//	{ p.print(s); return s; }
};

} // namespace type 
} // namespace sql 
} // namespace odb 

#endif
