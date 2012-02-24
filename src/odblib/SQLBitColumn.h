// File SQLBitColumn.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLBitColumn_H
#define SQLBitColumn_H

#include "SQLColumn.h"

namespace odb {
namespace sql {

class SQLBitColumn : public SQLColumn {
public:
	SQLBitColumn(SQLColumn&,const string&);
	~SQLBitColumn(); // Change to virtual if base class
private:
// No copy allowed
	SQLBitColumn(const SQLBitColumn&);
	SQLBitColumn& operator=(const SQLBitColumn&);
	
	string     field_;
	unsigned long mask_;
	unsigned long shift_;

// -- Overridden methods

	// From ODBIterator
	virtual void rewind();
	virtual double next(bool& missing);
	virtual void advance(unsigned long);
	virtual void print(ostream&) const; // Change to virtual if base class	

	//friend ostream& operator<<(ostream& s,const SQLBitColumn& p)
	//	{ p.print(s); return s; }
};

} // namespace sql
} // namespace odb

#endif
