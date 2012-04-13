#ifndef MDSetter_H
#define MDSetter_H

#include "odblib/IteratorProxy.h"
#include "MDUpdatingIterator.h"

namespace odb {

template <typename T>
class MDSetter
{
public:
	typedef typename odb::MDUpdatingIterator<T> iterator_class;
	typedef typename odb::IteratorProxy<iterator_class, MDSetter, const double> iterator;

	MDSetter(const T& b, const T& e, const vector<std::string>& columns, const vector<string>& types)
	: ii_(b), end_(e), columns_(columns), types_(types)
	{}

	~MDSetter() {}

	iterator begin() { return iterator(new iterator_class(ii_, end_, columns_, types_)); }
	const iterator end() { return iterator(new iterator_class(end_)); }

private:
	T ii_;
	const T& end_;
	const vector<std::string> columns_;
	const vector<std::string> types_;
};

} // namespace odb

#include "MDSetter.cc"

#endif

