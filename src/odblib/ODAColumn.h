/// \file ODAColumn.h

#ifndef ODAColumn_H
#define ODAColumn_H

#include "SQLColumn.h"

namespace odb {
namespace sql {

class ODAColumn : public SQLColumn {
public:
	ODAColumn(const type::SQLType&, SQLTable&, const string&, int, bool hasMissingValue, double missingValue, bool isBitfield,
const BitfieldDef& bitfieldDef, double*);
	~ODAColumn();

	void value(double* p) { value_ = p; }
	double * value() const { return value_; }

private:
	ODAColumn(const ODAColumn&);
	ODAColumn& operator=(const ODAColumn&);

	double* value_;
	double  missing_;

	virtual void rewind();
	virtual double next(bool& missing);
	virtual void advance(unsigned long);

// -- Friends
	//friend ostream& operator<<(ostream& s,const ODAColumn& p)
	//	{ p.print(s); return s; }
};

} // namespace sql 
} // namespace odb 

#endif
