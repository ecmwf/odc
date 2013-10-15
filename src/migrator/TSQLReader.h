/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef TSQLReader_H
#define TSQLReader_H

#include "eclib/PathName.h"
#include "odblib/IteratorProxy.h"

namespace odb {
namespace tool {

template <typename T>
class TSQLReader
{
public:
	typedef T iterator_class;
	typedef typename odb::IteratorProxy<iterator_class, TSQLReader, const double> iterator;

	TSQLReader(const std::string& pathName, const std::string& sql)
	: pathName_(pathName), sql_(sql)
	{}

	~TSQLReader() {}

	iterator begin()
    { 
        iterator_class* it = new iterator_class(pathName_, sql_);
        it->next();
        if (it->noMore_)
            eclib::Log::warning() << "ODBIterator::ODBIterator: result set empty, no data." << endl;
        return iterator(it);
    }

	const iterator end() { return iterator(0); }

#ifdef SWIGPYTHON
    // FIXME: add createReadIterator as in odb::Reader
    iterator __iter__() { return iterator(new iterator_class(pathName_, sql_)); }
#endif

private:
	const eclib::PathName pathName_;
	const std::string sql_;
};

} // namespace tool 
} // namespace odb 

#include "TSQLReader.cc"

#endif

