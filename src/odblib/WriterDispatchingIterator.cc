///
/// \file WriterDispatchingIterator.cc
///
/// @author Piotr Kuchta, Feb 2009

#include "oda.h"
#include "DataStream.h"
#include "Codec.h"
#include "Writer.h"
#include "WriterBufferingIterator.h"
#include "TemplateParameters.h"
#include "DataStream.h"
#include "UnsafeInMemoryDataHandle.h"
#include "FunctionEQ.h"

namespace odb {

template <typename WRITE_ITERATOR, typename OWNER>
string WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::generateFileName(const double* values, unsigned long count)
{
	string fileName = outputFileTemplate_;
	int diff = 0;
	for (TemplateParameters::iterator it = templateParameters_.begin(); it != templateParameters_.end(); ++it)
	{
		TemplateParameter& p = *(*it);

		// TODO: if values collected can be of different type then integer,
		// then below code must be updated:
        // code updated for string [19/07/2011] AF
		double d = values[p.columnIndex];
		string s;
        if ( columns_[p.columnIndex]->type() == odb::STRING)
		{
			char* sp = reinterpret_cast<char *>(&d);
			size_t len = 0;
			odb::sql::expression::function::FunctionEQ::trimStringInDouble(sp, len);
			s = string(sp, len);
			while (s.find("/") != string::npos)
			{
				string old = s;

				size_t pos = s.find("/");
				s.replace(pos, pos+1, string("__SLASH__"));

				//Log::info() << "WriterDispatchingIterator::generateFileName: '" << old << "' => '" << s << "'" << endl;
			}
        } else
		{
			int v = int(d);
			s = Translator<int, string>()(v);
        }

		fileName.replace(p.startPos - diff, p.endPos - p.startPos + 1, s);
		diff = outputFileTemplate_.size() - fileName.size();
	}

	//Log::debug() << "WriterDispatchingIterator::generateFileName: fileName = " << fileName <<  endl;
	return fileName;
}

template <typename WRITE_ITERATOR, typename OWNER>
WRITE_ITERATOR& WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::dispatch(const double* values, unsigned long count)
{
	Values dispatchedValues;
	for (size_t i = 0; i < dispatchedIndexes_.size(); ++i)
		dispatchedValues.push_back(values[dispatchedIndexes_[i]]);

	if (dispatchedValues == lastDispatchedValues_)
		return *iterators_[lastIndex_];

	Values2IteratorIndex::iterator p = values2iteratorIndex_.find(dispatchedValues);
	size_t iteratorIndex = (p != values2iteratorIndex_.end())
		? p->second
		: createIterator(dispatchedValues, generateFileName(values, count));

	lastDispatchedValues_ = dispatchedValues;
	lastIndex_ = iteratorIndex;

	lastDispatch_[iteratorIndex] = nrows_;

	return *iterators_[iteratorIndex];
}

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::createIterator(const Values& dispatchedValues, const std::string& fileName)
{
	//ostream& L(Log::debug());
	
	int iteratorIndex = iterators_.size();
	if (iterators_.size() >= maxOpenFiles_)
	{
		ASSERT(iterators_.size());

		size_t oldest = 0;
		unsigned long long oldestRow = lastDispatch_[oldest];
		for (size_t i = oldest; i < lastDispatch_.size(); ++i)
		{
			if (lastDispatch_[i] < oldestRow)
			{
				oldestRow = lastDispatch_[i];
				oldest = i;
			}
		}
		iteratorIndex = oldest;

		//L << "WriterDispatchingIterator::createIterator: evicted iterator " << iteratorIndex
		//	<< "' " << iteratorIndex2fileName_[iteratorIndex] << "' "
		//	<< " (oldest row: " << oldestRow << "), nrows_=" << nrows_ <<  endl;

		delete iterators_[iteratorIndex];
		iterators_[iteratorIndex] = 0;

		Values2IteratorIndex::iterator vit =  values2iteratorIndex_.begin();
		for (; vit != values2iteratorIndex_.end(); ++vit)
			if (vit->second == iteratorIndex)
				break;
		values2iteratorIndex_.erase(vit);
	}

	string operation;
	bool append = false;
    if (! PathName(fileName).exists())
	{
		filesCreated_[fileName] = 1;
		operation = "creating";
	}
	else
	{
		if (filesCreated_.find(fileName) == filesCreated_.end())
		{
			filesCreated_[fileName] = 1;
			operation = "overwriting";
		}
		else
		{
			append = true;
			filesCreated_[fileName]++;
			operation = "appending";
		}
	}

	//L << "WriterDispatchingIterator::dispatch: iterator " << iteratorIndex << ":" << operation << " '" << fileName << "'" << endl;

	if (iteratorIndex == iterators_.size())
		iterators_.push_back(iteratorsOwner_.createWriteIterator(fileName, append));
	else
		iterators_[iteratorIndex] = iteratorsOwner_.createWriteIterator(fileName, append);
	values2iteratorIndex_[dispatchedValues] = iteratorIndex;
	iteratorIndex2fileName_[iteratorIndex] = fileName;

	// Prop. metadata
	iterators_[iteratorIndex]->columns() = columns();
	iterators_[iteratorIndex]->writeHeader();

	return iteratorIndex;
} 

template <typename WRITE_ITERATOR, typename OWNER>
vector<PathName> WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::getFiles()
{
	vector<PathName> paths;
	for (map<string,int>::iterator it = filesCreated_.begin(); it != filesCreated_.end(); ++it)
		paths.push_back(it->first);
	return paths;
}

template <typename WRITE_ITERATOR, typename OWNER>
WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::WriterDispatchingIterator(OWNER &owner, int maxOpenFiles)
: buffer_(0),
  owner_(owner),
  iteratorsOwner_(),
  columns_(0),
  lastValues_(0),
  nextRow_(0),
  nrows_(0),
  outputFileTemplate_(owner_.outputFileTemplate()),
  properties_(),
  dispatchedIndexes_(),
  values2iteratorIndex_(),
  lastDispatch_(maxOpenFiles, -1),
  iteratorIndex2fileName_(maxOpenFiles),
  lastDispatchedValues_(),
  lastIndex_(),
  initialized_(false),
  refCount_(0),
  iterators_(),
  templateParameters_(),
  maxOpenFiles_(maxOpenFiles),
  filesCreated_()
{}

template <typename WRITE_ITERATOR, typename OWNER>
WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::~WriterDispatchingIterator()
{
	//Log::debug() << "WriterDispatchingIterator<WRITE_ITERATOR>::~WriterDispatchingIterator()" << endl;
	delete [] lastValues_;
	delete [] nextRow_;
	delete [] buffer_;
	for (size_t i = 0; i < iterators_.size(); ++i)
		delete iterators_[i];
}

template <typename WRITE_ITERATOR, typename OWNER>
unsigned long WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::gatherStats(const double* values, unsigned long count)
{
	return dispatch(values, count).gatherStats(values, count);
} 

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::setOptimalCodecs()
{
	int rc = 0;
	for (typename Iterators::iterator it = iterators_.begin(); it != iterators_.end(); ++it)
		rc |= (*it)->setOptimalCodecs();

	return rc; 
}

template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::writeHeader()
{
	//Log::debug() << "WriterDispatchingIterator<WRITE_ITERATOR>::writeHeader" << endl;

	delete [] lastValues_;
	delete [] nextRow_;
	int32_t count = columns().size();
	double* last = lastValues_ = new double [count];
	nextRow_ = new double [count];
	ASSERT(last);

	for (int i = 0; i < count; i++)
		nextRow_[i] = last[i] = columns_[i]->missingValue();

	nrows_ = 0;

	delete [] buffer_;
	buffer_ = new unsigned char[(count + 1) * sizeof(double)];
}

template <typename WRITE_ITERATOR, typename OWNER>
bool WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::next() { return writeRow(nextRow_, columns().size()) == 0; }

template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::parseTemplateParameters()
{
	templateParameters_.release();
	TemplateParameters::parse(outputFileTemplate_, templateParameters_, columns());
	if (templateParameters_.size() == 0)
	{
		stringstream ss;
		ss << "No parameters in output file template '" << outputFileTemplate_ << "'" << endl;
		throw UserError(ss.str());
	}
	dispatchedIndexes_.clear();
	for (size_t i = 0; i < templateParameters_.size(); ++i)
		dispatchedIndexes_.push_back(templateParameters_[i]->columnIndex);
	initialized_ = true;
}

template <typename WRITE_ITERATOR, typename OWNER>
double* WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::data() { return nextRow_; }

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::writeRow(const double* values, unsigned long count)
{
	if (!initialized_)
		parseTemplateParameters();

	WRITE_ITERATOR& wi = dispatch(values, count);
	int rc = wi.writeRow(values, count);

	if (rc == 0)	
		nrows_++;

	return rc;
} 

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::open()
{
	return 0;
}


template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::setColumn(size_t index, std::string name, ColumnType type)
{
	ASSERT(index < columns().size());
	Column* col = columns_[index];
	ASSERT(col);

	typedef DataStream<SameByteOrder, FastInMemoryDataHandle> DS;

	col->name(name); 
	col->type<DS>(type, false);
	//col->hasMissing(hasMissing);
	//col->missingValue(missingValue);

	for (typename Iterators::iterator it = iterators_.begin(); it != iterators_.end(); ++it)
		//(*it)->setColumn(index, name, type, hasMissing, missingValue);
		(*it)->setColumn(index, name, type);

	return 0;
}

template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::missingValue(size_t i, double missingValue)
{
	ASSERT(i < columns().size());
	Column* col = columns_[i];
	ASSERT(col);

	col->missingValue(missingValue);
}

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::setBitfieldColumn(size_t index, std::string name, ColumnType type, BitfieldDef b)
{
    //Log::info() << "WriterDispatchingIterator::setBitfieldColumn: " << endl;

    ASSERT(index < columns().size());
    Column* col = columns_[index];
    ASSERT(col);

    typedef DataStream<SameByteOrder, FastInMemoryDataHandle> DS;

    col->name(name);
    col->type<DS>(type, false);
    //col->hasMissing(hasMissing);
    //col->missingValue(missingValue);
    col->bitfieldDef(b);

    for (typename Iterators::iterator it = iterators_.begin(); it != iterators_.end(); ++it)
        //(*it)->setBitfieldColumn(index, name, type, b, hasMissing, missingValue);
        (*it)->setBitfieldColumn(index, name, type, b);

    return 0;
}

template <>
template <typename T>
unsigned long WriterDispatchingIterator<WriterBufferingIterator>::pass1(T& it, const T& end)
{
	Log::info() << "WriterDispatchingIterator<WriterBufferingIterator>::pass1:" << endl;

	if (! (it != end))
	{
		Log::warning() << "WriterDispatchingIterator<WriterBufferingIterator>::pass1: No input data." << endl;
		return 0;
	}

	// Copy columns from the input iterator.
	columns() = it->columns();

	if (!initialized_)
		parseTemplateParameters();

	size_t maxcols = columns().size();
	ASSERT(maxcols > 0);

	Log::debug() << "WriterDispatchingIterator::pass1<WriterBufferingIterator>: columns().size() => " << maxcols << endl;

	unsigned long nrows  = 0;
	for (; it != end; ++it)
	{
		if (it->isNewDataset() && columns() != it->columns() )
		{
			columns() = it->columns();

			parseTemplateParameters();

			for (size_t i = 0; i < iterators_.size(); ++i)
			{
				iterators_[i]->flush();
				iterators_[i]->columns() = columns();
				iterators_[i]->writeHeader();
			}
				
		}

		ASSERT(writeRow(it->data(), it->columns().size()) == 0);
		nrows++;
	} 

	Log::info() << "WriterDispatchingIterator<WriterBufferingIterator>::pass1: processed " << nrows << " row(s)." << endl;

	return nrows;
}

template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::property(string key, string value)
{
	// TODO: save property, make sure they are propagated to iterators as they are created
    properties_[key] = value;
}

template <typename WRITE_ITERATOR, typename OWNER>
string WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::property(string key)
{
    return properties_[key];
}

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::close()
{
	//Log::debug() << "WriterDispatchingIterator<WRITE_ITERATOR>::close()" << endl;
	int rc = 0;
	for (typename Iterators::iterator it = iterators_.begin(); it != iterators_.end(); ++it)
	{
		rc |= (*it)->close();
		delete *it;
	}
	iterators_.clear();

	return rc;
}

} // namespace odb 

