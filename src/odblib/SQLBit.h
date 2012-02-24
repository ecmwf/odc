// File SQLBit.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLBit_H
#define SQLBit_H

#include "SQLType.h"

namespace odb {
namespace sql {
namespace type {

class SQLBit : public SQLType {
public:
	SQLBit(const string& ,unsigned long ,unsigned long );
	~SQLBit(); // Change to virtual if base class

	unsigned long mask()  const { return mask_; }
	unsigned long shift() const { return shift_; }

private:
// No copy allowed
	SQLBit(const SQLBit&);
	SQLBit& operator=(const SQLBit&);

	unsigned long mask_;
	unsigned long shift_;

// -- Overridden methods
	// None

	virtual size_t size() const;
	void output(SQLOutput&, double, bool) const;
	virtual int getKind() const { return integerType; }

	//friend ostream& operator<<(ostream& s,const SQLBit& p)
	//	{ p.print(s); return s; }
};

} // namespace type
} // namespace sql 
} // namespace odb 

#endif
