// File ODADatabase.h
// Baudouin Raoult - ECMWF Oct 04

#ifndef ODADatabase_H
#define ODADatabase_H

class PathName;

#include "SQLDatabase.h"

namespace odb {
namespace sql {

class ODADatabase : public SQLDatabase {
public:
	ODADatabase(const PathName&,const string&);
	~ODADatabase(); // Change to virtual if base class

private:
// No copy allowed
	ODADatabase(const ODADatabase&);
	ODADatabase& operator=(const ODADatabase&);

// -- Methods
	// None

// -- Overridden methods
	// From SQLDatabase

	virtual void open();
	virtual void close();
	virtual SQLTable* table(const string&);
	virtual SQLTable* openDataHandle(DataHandle&, DataFormat = ODA);
	virtual SQLTable* openDataStream(istream&, DataFormat = CSV); 

// -- Friends
	//friend ostream& operator<<(ostream& s,const ODADatabase& p)
	//	{ p.print(s); return s; }
};

} // namespace sql 
} // namespace odb 

#endif
