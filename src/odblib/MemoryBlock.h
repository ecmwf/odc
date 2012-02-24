// File MemoryBlock.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef MemoryBlock_H
#define MemoryBlock_H

#include "machine.h"
#include "Exceptions.h"

// A simple class to implement buffers

class MemoryBlock {
public:
	MemoryBlock(size_t size);
	MemoryBlock(const string& s);
	MemoryBlock(const char*,size_t size);

	~MemoryBlock();

// -- Operators

	operator char*()                 { return (char*)buffer_; }
	operator const char*() const     { return (char*)buffer_; }

	operator unsigned char*()                 { return (unsigned char*)buffer_; }
	operator const unsigned char*() const     { return (unsigned char*)buffer_; }

	operator void*()                 { return buffer_; }
	operator const void*() const     { return buffer_; }

// -- Methods

	size_t size() const		 { return size_; }
	void size(size_t);

private:
// No copy allowed
	MemoryBlock(const MemoryBlock&);
	MemoryBlock& operator=(const MemoryBlock&);

	void create();
	void destroy();

	unsigned char *  buffer_;
	size_t size_;

};

#endif
