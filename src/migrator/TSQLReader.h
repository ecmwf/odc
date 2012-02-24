#ifndef TSQLReader_H
#define TSQLReader_H

#include "IteratorProxy.h"
#include "PathName.h"

namespace odb {
namespace tool {

template <typename T>
class TSQLReader
{
public:
	typedef T iterator_class;
	typedef typename odb::IteratorProxy<iterator_class, TSQLReader, const double> iterator;

	TSQLReader(const PathName& pathName, std::string sql)
	: pathName_(pathName), sql_(sql)
	{}

	~TSQLReader() {}

	iterator begin() { return iterator(new iterator_class(pathName_, sql_)); }
	const iterator end() { return iterator(0); }

private:
	const PathName pathName_;
	const std::string sql_;
};

} // namespace tool 
} // namespace odb 

#include "TSQLReader.cc"

#endif

