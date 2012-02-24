#include "MemoryBlock.h"
#include "Exceptions.h"

MemoryBlock::MemoryBlock(size_t size):
	buffer_(0),
	size_(size)
{
	create();
}

MemoryBlock::MemoryBlock(const char* p,size_t size):
	buffer_(0),
	size_(size)
{
	create();
	::memcpy(buffer_,p,size);
}

MemoryBlock::MemoryBlock(const string& s):
	buffer_(0),
	size_(s.length()+1)
{
	create();
	::strcpy((char*)buffer_,s.c_str());
}

void MemoryBlock::size(size_t newSize)
{
	destroy();
	size_ = newSize;
	create();
}

MemoryBlock::~MemoryBlock()
{ 
	destroy();
}

void MemoryBlock::create()
{
	buffer_ = new unsigned char[size_];
}

void MemoryBlock::destroy()
{
	delete [] buffer_;
}
