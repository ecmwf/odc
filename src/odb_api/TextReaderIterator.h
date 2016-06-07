/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file TextReaderIterator.h
///
/// @author Piotr Kuchta, Oct 2010

#ifndef TextReaderIterator_H
#define TextReaderIterator_H

#include "odb_api/Header.h"
#include "odb_api/IteratorProxy.h"
#include "odb_api/MetaData.h"

namespace eckit { class PathName; }
namespace eckit { class DataHandle; }
namespace eckit { class ExecutionContext; }

namespace odb {
	namespace sql {
		template <typename T> class SQLIteratorSession;
		class ODATableIterator;
	}
}

namespace odb {

class TextReader;

class TextReaderIterator 
{
public:
	TextReaderIterator (TextReader &owner);
	TextReaderIterator (TextReader &owner, const eckit::PathName&);
	~TextReaderIterator ();

	bool isNewDataset();
	const double* data();
	//long integer(int i);

	bool operator!=(const TextReaderIterator& other);

	MetaData& columns() { return columns_; }

	static odb::BitfieldDef parseBitfields(const std::string&);
//protected:

	int close();

// next() is public cause it needs to be used by the C API functions - normally client code should not use it
	bool next(ecml::ExecutionContext*);

private:
// No copy allowed.
    TextReaderIterator(const TextReaderIterator&);
    TextReaderIterator& operator=(const TextReaderIterator&);

	void initRowBuffer();
	void parseHeader();

	TextReader& owner_;
	MetaData columns_;
	double* lastValues_;
	unsigned long long nrows_;

	std::istream* in_;
	//eckit::DataHandle *f;
	//Properties properties_;

	bool newDataset_;
	bool noMore_;

	bool ownsF_;
public:
	ecml::ExecutionContext* context_;

	//ReadOnlyMemoryDataHandle memDataHandle_;

protected:
	// FIXME:
    TextReaderIterator(): owner_(*((TextReader *) 0)), columns_(0) {}

	int refCount_;

	//friend ::oda_write_iterator* ::oda_create_write_iterator(::oda*, const char *,int *); // for next()
	//friend int ::oda_read_iterator_get_next_row(::oda_read_iterator*, int, double*, int*);

	friend class odb::TextReader;
	friend class odb::IteratorProxy<odb::TextReaderIterator, odb::TextReader, const double>;
	friend class odb::Header<odb::TextReaderIterator>;
	friend class odb::sql::ODATableIterator;
};

} // namespace odb

#endif
