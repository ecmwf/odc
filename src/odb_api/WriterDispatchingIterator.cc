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
/// \file WriterDispatchingIterator.cc
///
/// @author Piotr Kuchta, Feb 2009

#include "odb_api/FunctionEQ.h"
#include "odb_api/Comparator.h"
#include "odb_api/Reader.h"
#include "eckit/log/Timer.h"
#include "eckit/utils/Translator.h"

namespace odb {

template <typename WRITE_ITERATOR, typename OWNER>
WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::WriterDispatchingIterator(OWNER &owner, int maxOpenFiles, bool append)
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
  files_(),
  templateParameters_(),
  maxOpenFiles_(maxOpenFiles),
  filesCreated_(),
  append_(append)
{}


template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::setColumn(size_t index, std::string name, ColumnType type)
{
	ASSERT(index < columns().size());
	Column* col = columns_[index];
	ASSERT(col);

	col->name(name);
	col->type<DataStream<SameByteOrder, FastInMemoryDataHandle> >(type, false);
	return 0;
}

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::setBitfieldColumn(size_t index, std::string name, ColumnType type, BitfieldDef b)
{
	ASSERT(index < columns().size());
	Column* col = columns_[index];
	ASSERT(col);

	col->name(name);
	col->type<DataStream<SameByteOrder, FastInMemoryDataHandle> >(type, false);
    col->bitfieldDef(b);
	col->missingValue(0);
	return 0;
}

template <typename WRITE_ITERATOR, typename OWNER>
std::string WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::generateFileName(const double* values, unsigned long count)
{
    std::string fileName (outputFileTemplate_);
    int diff (0);
    for (TemplateParameters::iterator it (templateParameters_.begin()); it != templateParameters_.end(); ++it)
    {
        TemplateParameter& p (*(*it));

        // TODO: if values collected can be of different type then integer,
        // then below code must be updated:
        // code updated for std::string [19/07/2011] AF
        double d (values[p.columnIndex]);
        std::string s;
        if ( columns_[p.columnIndex]->type() == odb::STRING)
        {
            char* sp (reinterpret_cast<char *>(&d));
            size_t len (0);
            odb::sql::expression::function::FunctionEQ::trimStringInDouble(sp, len);
            s = std::string(sp, len);
            while (s.find("/") != std::string::npos)
            {
                std::string old (s);
                size_t pos (s.find("/"));
                s.replace(pos, pos+1, std::string("__SLASH__"));
                //eckit::Log::info() << "WriterDispatchingIterator::generateFileName: '" << old << "' => '" << s << "'" << std::endl;
            }
        } else
        {
            s = eckit::Translator<int, std::string>()(int(d));
        }

        fileName.replace(p.startPos - diff, p.endPos - p.startPos + 1, s);
        diff = outputFileTemplate_.size() - fileName.size();
    }

    //eckit::Log::debug() << "WriterDispatchingIterator::generateFileName: fileName = " << fileName <<  std::endl;
    return fileName;
}

template <typename WRITE_ITERATOR, typename OWNER>
WRITE_ITERATOR& WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::dispatch(const double* values, unsigned long count)
{
    return *iterators_[this->dispatchIndex(values, count)];
}

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::dispatchIndex(const double* values, unsigned long count)
{
    Values dispatchedValues;
    for (size_t i (0); i < dispatchedIndexes_.size(); ++i)
        dispatchedValues.push_back(values[dispatchedIndexes_[i]]);

    if (dispatchedValues == lastDispatchedValues_)
        return lastIndex_;

    Values2IteratorIndex::iterator p (values2iteratorIndex_.find(dispatchedValues));
    size_t iteratorIndex ((p != values2iteratorIndex_.end())
                           ? p->second
                           : createIterator(dispatchedValues, generateFileName(values, count), values, count));

    lastDispatchedValues_ = dispatchedValues;
    lastIndex_ = iteratorIndex;
    lastDispatch_[iteratorIndex] = nrows_;
    return iteratorIndex;
}

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::createIterator(const Values& dispatchedValues, const std::string& fileName,
const double* values, unsigned long count)
{
    std::ostream& L(eckit::Log::info());

    int iteratorIndex (iterators_.size());
    if (iterators_.size() >= maxOpenFiles_)
    {
        ASSERT(iterators_.size());

        size_t oldest (0);
        unsigned long long oldestRow (lastDispatch_[oldest]);
        for (size_t i = oldest; i < lastDispatch_.size(); ++i)
        {
            if (lastDispatch_[i] < oldestRow)
            {
                oldestRow = lastDispatch_[i];
                oldest = i;
            }
        }
		iteratorIndex = oldest;

		L << "split writer: evicted iterator " << iteratorIndex
			<< "' " << iteratorIndex2fileName_[iteratorIndex] << "' "
			<< " (oldest row: " << oldestRow << "), nrows_=" << nrows_ <<  std::endl;

        delete iterators_[iteratorIndex];
        iterators_[iteratorIndex] = 0;

        Values2IteratorIndex::iterator vit (values2iteratorIndex_.begin());
        for (; vit != values2iteratorIndex_.end(); ++vit)
            if (vit->second == iteratorIndex)
                break;
        values2iteratorIndex_.erase(vit);
    }

    std::string operation;
    //bool append = false;
    if (append_ || !eckit::PathName(fileName).exists())
    {
        filesCreated_[fileName] = 1;
        operation = "creating";
    }
    else
    {
        if (filesCreated_.find(fileName) == filesCreated_.end())
        {
            filesCreated_[fileName] = 1; operation = "overwriting";
        }
        else
        {
            append_ = true;
            filesCreated_[fileName]++; operation = "appending";
        }
    }

    L << iteratorIndex << ": " << operation << " '" << fileName << "'" << std::endl;

    if (iteratorIndex == iterators_.size())
    {
        iterators_.push_back(iteratorsOwner_.createWriteIterator(fileName, append_));
        files_.push_back(fileName);
    }
    else
    {
        iterators_[iteratorIndex] = iteratorsOwner_.createWriteIterator(fileName, append_);
        files_[iteratorIndex] = fileName;
        //ASSERT(files_[iteratorIndex] == fileName);
    }
    values2iteratorIndex_[dispatchedValues] = iteratorIndex;
    iteratorIndex2fileName_[iteratorIndex] = fileName;

    // Prop. metadata
    iterators_[iteratorIndex]->columns(columns());
    //iterators_[iteratorIndex]->writeHeader();
    //iterators_[iteratorIndex]->allocBuffers();
    //iterators_[iteratorIndex]->gatherStats(values, count);

    return iteratorIndex;
}

template <typename WRITE_ITERATOR, typename OWNER>
std::vector<eckit::PathName> WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::outputFiles()
{
    std::vector<eckit::PathName> paths;
    for (std::map<std::string,int>::iterator it (filesCreated_.begin()); it != filesCreated_.end(); ++it)
        paths.push_back(it->first);
    return paths;
}

template <typename WRITE_ITERATOR, typename OWNER>
WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::~WriterDispatchingIterator()
{
    //eckit::Log::debug() << "WriterDispatchingIterator<WRITE_ITERATOR>::~WriterDispatchingIterator()" << std::endl;
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
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::writeHeader()
{
	//eckit::Log::debug() << "WriterDispatchingIterator<WRITE_ITERATOR>::writeHeader" << std::endl;

    delete [] lastValues_;
    delete [] nextRow_;
    int32_t count (columns().size());
    double* last (lastValues_ = new double [count]);
    nextRow_ = new double [count];
    ASSERT(last);

    for (int i (0); i < count; i++)
        nextRow_[i] = last[i] = columns_[i]->missingValue();

    nrows_ = 0;

    delete [] buffer_;
    buffer_ = new unsigned char[(count + 1) * sizeof(double)];
}

template <typename WRITE_ITERATOR, typename OWNER>
bool WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::next(eckit::ExecutionContext*)
{
    return writeRow(nextRow_, columns().size()) == 0;
}

template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::parseTemplateParameters()
{
    templateParameters_.release();
    TemplateParameters::parse(outputFileTemplate_, templateParameters_, columns());
    if (templateParameters_.size() == 0)
    {
        std::stringstream ss;
        ss << "No parameters in output file template '" << outputFileTemplate_ << "'" << std::endl;
        throw eckit::UserError(ss.str());
    }
    dispatchedIndexes_.clear();
    for (size_t i (0); i < templateParameters_.size(); ++i)
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
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::open() { return 0; }

/* * /
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
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::setBitfieldColumn(size_t index, std::string name, ColumnType type, BitfieldDef b)
{
    //eckit::Log::info() << "WriterDispatchingIterator::setBitfieldColumn: " << std::endl;

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
/ *
*/

template <typename WRITE_ITERATOR, typename OWNER>
const MetaData& WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::columns(const MetaData& md)
{
    columns_ = md;
    return md;
}

template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::missingValue(size_t i, double missingValue)
{
	ASSERT(i < columns().size());
	Column* col (columns_[i]);
	ASSERT(col);

	col->missingValue(missingValue);
}

template <>
template <typename T>
unsigned long WriterDispatchingIterator<WriterBufferingIterator,DispatchingWriter>::pass1(T& it, const T& end)
{
	if (! (it != end))
	{
		eckit::Log::warning() << "Split: No input data." << std::endl;
		return 0;
	}

	// Copy columns from the input iterator.
	columns(it->columns());

	if (!initialized_) parseTemplateParameters();

	size_t maxcols = columns().size();
	ASSERT(maxcols > 0);

	eckit::Log::debug() << "WriterDispatchingIterator::pass1<WriterBufferingIterator>: columns().size() => " << maxcols << std::endl;

	nrows_  = 0;
	for (; it != end; ++it)
	{
		if (it->isNewDataset() && columns() != it->columns() )
		{
			columns(it->columns());
			parseTemplateParameters();

			for (size_t i = 0; i < iterators_.size(); ++i)
			{
				iterators_[i]->flush();
				iterators_[i]->columns(columns());
				iterators_[i]->writeHeader();
			}
		}

		const double* data (it->data());
		size_t size (it->columns().size());
		int rc (writeRow(data, size));
		ASSERT(rc == 0);
	}

	eckit::Log::info() << "Split: processed " << nrows_ << " row(s)." << std::endl;
	return nrows_;
}

template <>
template <typename T>
void WriterDispatchingIterator<WriterBufferingIterator,DispatchingWriter>::verify(T& it, const T& end) {
    using namespace eckit;
    using namespace std;
    Log::info() << "Verifying split..." << endl;
    Timer timer("Split verification");

    vector<Reader*> readers;
    vector<pair<Reader::iterator, Reader::iterator> > iterators;
    for (size_t i (0); i < files_.size(); ++i) {
        Log::info() << "Opening '" << files_[i] << "'" << endl;
        Reader* reader(new Reader(files_[i]));
        readers.push_back(reader);
        iterators.push_back(make_pair(reader->begin(), reader->end()));
    }

    vector<size_t> rowsRead(files_.size());
    Comparator comparator;
    unsigned long numberOfDifferences (0);
    long long i (0);
    for (; it != end; ++i)
    {
		//if (it->isNewDataset() && columns() != it->columns() )
		if (columns() != it->columns())
		{
			columns(it->columns());
			parseTemplateParameters();
        }

        size_t fileIndex(dispatchIndex(it->data(), it->columns().size()));
        const std::string& outFileName (files_[fileIndex]);

        size_t n(columns().size());
        typedef Reader::iterator I;
        std::pair<I, I>& its(iterators[fileIndex]);
        I& sIt(its.first), sEnd(its.second);

        const MetaData& sMetaData (sIt->columns());
        try {
            ASSERT(sIt != sEnd && sMetaData == columns());

            ++rowsRead[fileIndex];
            const double* const& originalData(it->data());
            const double* const& outputData(sIt->data());
            comparator.compare(n, originalData, outputData, columns(), sMetaData);
        } catch (...) {
            ++numberOfDifferences;
            Log::info() << "Row " << i << " of input (" << rowsRead[fileIndex] << " of " << outFileName << ") not correct." << endl << endl;
        }
        ++it;
        ++sIt;
    }
    Log::info() << "Number of rows: " << i << ". Total number of differences: " << numberOfDifferences  << std::endl;
    ASSERT(! (it != end));

    for (size_t j = 0; j < readers.size(); ++j)
        delete readers[j];
}

template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::property(std::string key, std::string value)
{
	// TODO: save property, make sure they are propagated to iterators as they are created
    properties_[key] = value;
}

template <typename WRITE_ITERATOR, typename OWNER>
std::string WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::property(std::string key) { return properties_[key]; }

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::close()
{
	//eckit::Log::debug() << "WriterDispatchingIterator<WRITE_ITERATOR>::close()" << std::endl;
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

