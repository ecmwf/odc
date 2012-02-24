// File MD5.h
// Baudouin Raoult - ECMWF Dec 04

#ifndef MD5_H
#define MD5_H

extern "C" {
#include "md5.h"
} // extern "C"

#include "machine.h"

// Forward declarations

class MD5 {
public:
	MD5();
	~MD5(); // Change to virtual if base class

	void add(const void*, long);
	string digest();

private:
// No copy allowed
	MD5(const MD5&);
	MD5& operator=(const MD5&);

	md5_state state_;
	string digest_;
};

#endif
