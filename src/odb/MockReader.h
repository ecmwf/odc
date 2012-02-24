#ifndef MockReader_H
#define MockReader_H

#include "oda.h"

namespace odb {
namespace tool {

template <typename T>
class MockReader
{
public:
	typedef T iterator_class;
	typedef odb::IteratorProxy<T, MockReader, const double> iterator;

	iterator begin() { return iterator(new T); }
	const iterator end() { return iterator(0); }
};

#include "MockReader.cc"

} // namespace tool 
} // namespace odb 

#endif

