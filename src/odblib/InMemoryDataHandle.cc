#include "machine.h"
#include "InMemoryDataHandle.h"
#include "FileHandle.h"
#include "Timer.h"
#include "Bytes.h"

namespace odb {

InMemoryDataHandle::InMemoryDataHandle()
: buf_(), readIterator_(buf_.begin())
{}

InMemoryDataHandle::InMemoryDataHandle(const MemoryBlock& buffer)
: buf_((const unsigned char *) buffer, (const unsigned char *) buffer + buffer.size()),
  readIterator_(buf_.begin())
{
}

void InMemoryDataHandle::buffer(const MemoryBlock& buffer)
{
	buf_ = vector<unsigned char>((const unsigned char *) buffer, (const unsigned char *) buffer + buffer.size());
	readIterator_ = buf_.begin();
}

InMemoryDataHandle::~InMemoryDataHandle()
{}

} // namespace odb

