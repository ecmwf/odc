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
/// \file WriterDispatchingIterator.h
///
/// @author Piotr Kuchta, June 2009

#ifndef WriterDispatchingIterator_H
#define WriterDispatchingIterator_H

//#include "odblib/DispatchingWriter.h"
//#include "odblib/WriterBufferingIterator.h"

#include "odblib/RowsIterator.h"
#include "odblib/TemplateParameters.h"
#include "odblib/Writer.h"

class PathName;
class DataHandle;

namespace odb {

class HashTable;
class SQLIteratorSession;
class TemplateParameters;

//template <typename WRITE_ITERATOR = WriterBufferingIterator, typename OWNER = DispatchingWriter>
template <typename WRITE_ITERATOR, typename OWNER >
class WriterDispatchingIterator : public RowsWriterIterator
{
	typedef vector<double> Values;
	typedef map<Values,int> Values2IteratorIndex;
	typedef vector<WRITE_ITERATOR *> Iterators;
public:
	WriterDispatchingIterator (OWNER &owner, int maxOpenFiles);
	~WriterDispatchingIterator();

	int open();

	double* data();

	virtual int setColumn(size_t index, std::string name, ColumnType type);
	virtual int setBitfieldColumn(size_t index, std::string name, ColumnType type, BitfieldDef b);

	void missingValue(size_t i, double); 

	template <typename T> unsigned long pass1(T&, const T&);
	template <typename T> void verify(T&, const T&);
	unsigned long gatherStats(const double* values, unsigned long count);

	virtual int close();

	ColumnType columnType(size_t index);
	const std::string& columnName(size_t index) const;
	const std::string& codecName(size_t index) const;
	double columnMissingValue(size_t index);

	virtual MetaData& columns() { return columns_; }

	OWNER& owner() { return owner_; }

	void property(string key, string value);
	string property(string);

	vector<PathName> getFiles();

//protected:
	int setOptimalCodecs();
	void writeHeader();

	int writeRow(const double* values, unsigned long count);

protected:
	bool next();

	/// Find iterator data should be dispatched to.
	WRITE_ITERATOR& dispatch(const double* values, unsigned long count);
	int createIterator(const Values& dispatchedValues, const std::string& fileName);

	string generateFileName(const double* values, unsigned long count);

	unsigned char *buffer_;
	OWNER& owner_;
	Writer<WRITE_ITERATOR> iteratorsOwner_;
	MetaData columns_;
	double* lastValues_;
	double* nextRow_;
	unsigned long long nrows_;
	string outputFileTemplate_;

	Properties properties_;

	vector<int> dispatchedIndexes_;
	Values2IteratorIndex values2iteratorIndex_;
	vector<unsigned long long> lastDispatch_;
	vector<string> iteratorIndex2fileName_;

	Values lastDispatchedValues_;
	int lastIndex_;
	bool initialized_;

private:
// No copy allowed.
	WriterDispatchingIterator(const WriterDispatchingIterator&);
	WriterDispatchingIterator& operator=(const WriterDispatchingIterator&);

	void parseTemplateParameters();

	int refCount_;

	Iterators iterators_;
	vector<PathName> files_;

	TemplateParameters templateParameters_;
	int maxOpenFiles_;

	map<string,int> filesCreated_;
	vector<unsigned int> rowsOutputFileIndex_;

	friend class IteratorProxy<WriterDispatchingIterator<WRITE_ITERATOR,DispatchingWriter>, DispatchingWriter>;
};

} // namespace odb 

#include "odblib/WriterDispatchingIterator.cc"

#endif
