///
/// \file WriterDispatchingIterator.h
///
/// @author Piotr Kuchta, June 2009

#ifndef WriterDispatchingIterator_H
#define WriterDispatchingIterator_H

//#include "DispatchingWriter.h"

class PathName;
class DataHandle;

namespace odb {

class HashTable;
class SQLIteratorSession;
class TemplateParameters;

template <typename WRITE_ITERATOR = WriterBufferingIterator, typename OWNER = DispatchingWriter>
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

	TemplateParameters templateParameters_;
	int maxOpenFiles_;

	map<string,int> filesCreated_;

	friend class IteratorProxy<WriterDispatchingIterator<WRITE_ITERATOR>, DispatchingWriter>;
};

} // namespace odb 

#include "WriterDispatchingIterator.cc"

#endif
