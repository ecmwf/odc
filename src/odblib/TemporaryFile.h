// File TemporaryFile.h
// Baudouin Raoult - ECMWF Sep 01

#ifndef TemporaryFile_H
#define TemporaryFile_H

#include "eclib/PathName.h"

class TemporaryFile : public PathName {
public:
	TemporaryFile();
	~TemporaryFile(); // Change to virtual if base class
private:
// No copy allowed
	TemporaryFile(const TemporaryFile&);
	TemporaryFile& operator=(const TemporaryFile&);

	//friend ostream& operator<<(ostream& s,const TemporaryFile& p)
	//	{ p.print(s); return s; }
};

#endif
