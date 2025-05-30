/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"
#include "eckit/utils/Translator.h"

#include "odc/Comparator.h"
#include "odc/LibOdc.h"
#include "odc/Reader.h"
#include "odc/WriterDispatchingIterator.h"

using namespace odc::core;


namespace {
// n.b. Duplicated from eckit::sql::expression::function::FunctionEQ::trimStringInDouble.
//      TODO: Put somewhere better.
void trimStringInDouble(char*& p, size_t& len) {
    len = 0;
    for (; len < sizeof(double) && isprint(p[len]); ++len)
        ;
    for (; len > 0 && isspace(p[len - 1]); --len)
        ;
    size_t plen = len;
    for (char* pp = p; isspace(*p) && p < pp + plen;) {
        ++p;
        --len;
    }
}
}  // namespace

namespace odc {

//----------------------------------------------------------------------------------------------------------------------


template <typename WRITE_ITERATOR, typename OWNER>
WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::WriterDispatchingIterator(OWNER& owner, int maxOpenFiles,
                                                                            bool append) :
    owner_(owner),
    iteratorsOwner_(),
    columns_(0),
    lastValues_(0),
    nextRow_(0),
    columnOffsets_(0),
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
    append_(append),
    refCount_(0),
    iterators_(),
    files_(),
    templateParameters_(),
    maxOpenFiles_(maxOpenFiles),
    filesCreated_() {}


template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::setNumberOfColumns(size_t n) {
    columns_.setSize(n);
}

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::setColumn(size_t index, std::string name, api::ColumnType type) {
    ASSERT(index < columns().size());
    Column* col = columns_[index];
    ASSERT(col);

    col->name(name);
    col->type<SameByteOrder>(type);
    return 0;
}

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::setBitfieldColumn(size_t index, std::string name,
                                                                        api::ColumnType type,
                                                                        eckit::sql::BitfieldDef b) {
    ASSERT(index < columns().size());
    Column* col = columns_[index];
    ASSERT(col);

    col->name(name);
    col->type<SameByteOrder>(type);
    col->bitfieldDef(b);
    return 0;
}

template <typename WRITE_ITERATOR, typename OWNER>
std::string WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::generateFileName(const double* values,
                                                                               unsigned long count) {
    std::string fileName(outputFileTemplate_);
    int diff(0);
    for (TemplateParameters::iterator it(templateParameters_.begin()); it != templateParameters_.end(); ++it) {
        TemplateParameter& p(*(*it));

        // TODO: if values collected can be of different type then integer,
        // then below code must be updated:
        // code updated for std::string [19/07/2011] AF
        double d(values[p.columnIndex]);
        std::string s;
        if (columns_[p.columnIndex]->type() == api::STRING) {
            char* sp(reinterpret_cast<char*>(&d));
            size_t len(0);
            trimStringInDouble(sp, len);
            s = std::string(sp, len);
            while (s.find("/") != std::string::npos) {
                std::string old(s);
                size_t pos(s.find("/"));
                s.replace(pos, pos + 1, std::string("__SLASH__"));
                // eckit::Log::info() << "WriterDispatchingIterator::generateFileName: '" << old << "' => '" << s << "'"
                // << std::endl;
            }
        }
        else {
            s = eckit::Translator<int, std::string>()(int(d));
        }

        fileName.replace(p.startPos - diff, p.endPos - p.startPos + 1, s);
        diff = outputFileTemplate_.size() - fileName.size();
    }

    // LOG_DEBUG_LIB(LibOdc) << "WriterDispatchingIterator::generateFileName: fileName = " << fileName <<  std::endl;
    return fileName;
}

template <typename WRITE_ITERATOR, typename OWNER>
WRITE_ITERATOR& WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::dispatch(const double* values, unsigned long count) {
    return *iterators_[this->dispatchIndex(values, count)];
}

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::dispatchIndex(const double* values, unsigned long count) {
    Values dispatchedValues;
    for (size_t i(0); i < dispatchedIndexes_.size(); ++i)
        dispatchedValues.push_back(values[dispatchedIndexes_[i]]);

    if (dispatchedValues == lastDispatchedValues_)
        return lastIndex_;

    Values2IteratorIndex::iterator p(values2iteratorIndex_.find(dispatchedValues));
    size_t iteratorIndex((p != values2iteratorIndex_.end())
                             ? p->second
                             : createIterator(dispatchedValues, generateFileName(values, count)));

    lastDispatchedValues_        = dispatchedValues;
    lastIndex_                   = iteratorIndex;
    lastDispatch_[iteratorIndex] = nrows_;
    return iteratorIndex;
}

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::createIterator(const Values& dispatchedValues,
                                                                     const std::string& fileName) {
    int iteratorIndex(iterators_.size());
    if (iterators_.size() >= maxOpenFiles_) {
        ASSERT(iterators_.size());

        size_t oldest(0);
        unsigned long long oldestRow(lastDispatch_[oldest]);
        for (size_t i = oldest; i < lastDispatch_.size(); ++i) {
            if (lastDispatch_[i] < oldestRow) {
                oldestRow = lastDispatch_[i];
                oldest    = i;
            }
        }
        iteratorIndex = oldest;

        LOG_DEBUG_LIB(LibOdc) << "split writer: evicted iterator " << iteratorIndex << "' "
                              << iteratorIndex2fileName_[iteratorIndex] << "' "
                              << " (oldest row: " << oldestRow << "), nrows_=" << nrows_ << std::endl;

        delete iterators_[iteratorIndex];
        iterators_[iteratorIndex] = 0;

        Values2IteratorIndex::iterator vit(values2iteratorIndex_.begin());
        for (; vit != values2iteratorIndex_.end(); ++vit)
            if (vit->second == iteratorIndex)
                break;
        values2iteratorIndex_.erase(vit);
    }

    std::string operation;
    // bool append = false;
    if (append_ || !eckit::PathName(fileName).exists()) {
        filesCreated_[fileName] = 1;
        operation               = "creating";
    }
    else {
        if (filesCreated_.find(fileName) == filesCreated_.end()) {
            filesCreated_[fileName] = 1;
            operation               = "overwriting";
        }
        else {
            append_ = true;
            filesCreated_[fileName]++;
            operation = "appending";
        }
    }

    LOG_DEBUG_LIB(LibOdc) << iteratorIndex << ": " << operation << " '" << fileName << "'" << std::endl;

    if (iteratorIndex == iterators_.size()) {
        iterators_.push_back(iteratorsOwner_.createWriteIterator(fileName, append_));
        files_.push_back(fileName);
    }
    else {
        iterators_[iteratorIndex] = iteratorsOwner_.createWriteIterator(fileName, append_);
        files_[iteratorIndex]     = fileName;
        // ASSERT(files_[iteratorIndex] == fileName);
    }
    values2iteratorIndex_[dispatchedValues] = iteratorIndex;
    iteratorIndex2fileName_[iteratorIndex]  = fileName;

    // Prop. metadata
    iterators_[iteratorIndex]->columns(columns());
    iterators_[iteratorIndex]->writeHeader();

    return iteratorIndex;
}

template <typename WRITE_ITERATOR, typename OWNER>
std::vector<eckit::PathName> WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::outputFiles() {
    std::vector<eckit::PathName> paths;
    for (std::map<std::string, int>::iterator it(filesCreated_.begin()); it != filesCreated_.end(); ++it)
        paths.push_back(it->first);
    return paths;
}

template <typename WRITE_ITERATOR, typename OWNER>
WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::~WriterDispatchingIterator() {
    // LOG_DEBUG_LIB(LibOdc) << "WriterDispatchingIterator<WRITE_ITERATOR>::~WriterDispatchingIterator()" << std::endl;
    delete[] lastValues_;
    delete[] nextRow_;
    delete[] columnOffsets_;
    for (size_t i = 0; i < iterators_.size(); ++i)
        delete iterators_[i];
}

template <typename WRITE_ITERATOR, typename OWNER>
unsigned long WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::gatherStats(const double* values, unsigned long count) {
    return dispatch(values, count).gatherStats(values, count);
}

template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::writeHeader() {
    // LOG_DEBUG_LIB(LibOdc) << "WriterDispatchingIterator<WRITE_ITERATOR>::writeHeader" << std::endl;

    delete[] lastValues_;
    delete[] nextRow_;
    delete[] columnOffsets_;

    int32_t numDoubles = rowDataSizeDoubles();
    int32_t count      = columns().size();

    lastValues_    = new double[numDoubles];
    nextRow_       = new double[numDoubles];
    columnOffsets_ = new size_t[count];
    ASSERT(lastValues_);

    size_t offset = 0;
    for (int i(0); i < count; i++) {
        nextRow_[i] = lastValues_[i] = columns_[i]->missingValue();
        columnOffsets_[i]            = offset;
        offset += columns_[i]->dataSizeDoubles();
    }

    nrows_ = 0;
}

template <typename WRITE_ITERATOR, typename OWNER>
bool WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::next() {
    return writeRow(nextRow_, columns().size()) == 0;
}

template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::parseTemplateParameters() {
    templateParameters_.reset();
    TemplateParameters::parse(outputFileTemplate_, templateParameters_, columns());
    if (templateParameters_.size() == 0) {
        std::stringstream ss;
        ss << "No parameters in output file template '" << outputFileTemplate_ << "'" << std::endl;
        throw eckit::UserError(ss.str());
    }
    dispatchedIndexes_.clear();
    for (size_t i(0); i < templateParameters_.size(); ++i)
        dispatchedIndexes_.push_back(templateParameters_[i]->columnIndex);
    initialized_ = true;
}

template <typename WRITE_ITERATOR, typename OWNER>
double* WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::data() {
    return nextRow_;
}

template <typename WRITE_ITERATOR, typename OWNER>
double& WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::data(size_t i) {
    return nextRow_[columnOffsets_[i]];
}

template <typename WRITE_ITERATOR, typename OWNER>
size_t WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::rowDataSizeDoubles() const {

    size_t total = 0;
    for (const auto& column : columns()) {
        total += column->dataSizeDoubles();
    }
    return total;
}


template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::writeRow(const double* values, unsigned long count) {
    if (!initialized_)
        parseTemplateParameters();

    WRITE_ITERATOR& wi = dispatch(values, count);
    int rc             = wi.writeRow(values, count);

    if (rc == 0)
        nrows_++;

    return rc;
}

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::open() {
    return 0;
}

/* * /
template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::setColumn(size_t index, std::string name, api::ColumnType type)
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
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::setBitfieldColumn(size_t index, std::string name, api::ColumnType
type, eckit::sql::BitfieldDef b)
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
const MetaData& WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::columns(const MetaData& md) {
    columns_ = md;
    return md;
}

template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::missingValue(size_t i, double missingValue) {
    ASSERT(i < columns().size());
    Column* col(columns_[i]);
    ASSERT(col);

    col->missingValue(missingValue);
}

template <>
template <typename T>
unsigned long WriterDispatchingIterator<WriterBufferingIterator, DispatchingWriter>::pass1(T& it, const T& end) {
    if (!(it != end)) {
        eckit::Log::warning() << "Split: No input data." << std::endl;
        return 0;
    }

    // Copy columns from the input iterator.
    columns(it->columns());

    if (!initialized_)
        parseTemplateParameters();

    size_t maxcols = columns().size();
    ASSERT(maxcols > 0);

    LOG_DEBUG_LIB(LibOdc) << "WriterDispatchingIterator::pass1<WriterBufferingIterator>: columns().size() => "
                          << maxcols << std::endl;

    nrows_ = 0;
    for (; it != end; ++it) {
        if (it->isNewDataset() && columns() != it->columns()) {
            columns(it->columns());
            parseTemplateParameters();

            for (size_t i = 0; i < iterators_.size(); ++i) {
                iterators_[i]->flush();
                iterators_[i]->columns(columns());
                iterators_[i]->writeHeader();
            }
        }

        const double* data(it->data());
        size_t size(it->columns().size());
        int rc(writeRow(data, size));
        ASSERT(rc == 0);
    }

    LOG_DEBUG_LIB(LibOdc) << "Split: processed " << nrows_ << " row(s)." << std::endl;
    return nrows_;
}

template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::flushAndResetColumnSizes(
    const std::map<std::string, size_t>& resetColumnSizeDoubles) {
    for (size_t i = 0; i < iterators_.size(); ++i) {
        iterators_[i]->flushAndResetColumnSizes(resetColumnSizeDoubles);
    }
}


template <>
template <typename T>
void WriterDispatchingIterator<WriterBufferingIterator, DispatchingWriter>::verify(T& it, const T& end) {
    using namespace eckit;
    using namespace std;
    Log::info() << "Verifying split..." << endl;
    Timer timer("Split verification");

    // Copy columns from the input iterator.
    columns(it->columns());

    vector<Reader*> readers;
    vector<pair<Reader::iterator, Reader::iterator> > iterators;
    for (size_t i(0); i < files_.size(); ++i) {
        Reader* reader(new Reader(files_[i]));
        readers.push_back(reader);
        iterators.push_back(make_pair(reader->begin(), reader->end()));
    }

    vector<size_t> rowsRead(files_.size());

    // Frames in the source/dispatched cases will have different sizes, so the haveMissing()
    // value may differ. Only compare the values
    bool skipTestingHaveMissing = true;
    Comparator comparator(skipTestingHaveMissing);

    unsigned long numberOfDifferences(0);
    long long i(0);
    for (; it != end; ++i) {
        if (it->isNewDataset() && columns() != it->columns()) {
            columns(it->columns());
            parseTemplateParameters();
        }

        size_t fileIndex(dispatchIndex(it->data(), it->columns().size()));
        const std::string& outFileName(files_[fileIndex]);

        size_t n(columns().size());
        typedef Reader::iterator I;
        std::pair<I, I>& its(iterators[fileIndex]);
        I &sIt(its.first), sEnd(its.second);

        const MetaData& sMetaData(sIt->columns());
        try {
            ASSERT(sIt != sEnd);

            // In the parent codec, we will always have the largest data size (e.g. for
            // the longest string). This will not be true in the dispatched iterators
            // which will only be large enough for the strings dispatched to them.
            bool compareDataSizes = false;
            ASSERT(sMetaData.equals(columns(), compareDataSizes));

            ++rowsRead[fileIndex];
            const double* const originalData(it->data());
            const double* const outputData(sIt->data());
            comparator.compare(n, originalData, outputData, columns(), sMetaData);
        }
        catch (...) {
            ++numberOfDifferences;
            Log::info() << "Row " << i << " of input (" << rowsRead[fileIndex] << " of " << outFileName
                        << ") not correct." << endl
                        << endl;
        }
        ++it;
        ++sIt;
    }
    Log::info() << "Number of rows: " << i << ". Total number of differences: " << numberOfDifferences << std::endl;
    ASSERT(!(it != end));

    for (size_t j = 0; j < readers.size(); ++j)
        delete readers[j];
}

template <typename WRITE_ITERATOR, typename OWNER>
void WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::property(std::string key, std::string value) {
    // TODO: save property, make sure they are propagated to iterators as they are created
    properties_[key] = value;
}

template <typename WRITE_ITERATOR, typename OWNER>
std::string WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::property(std::string key) {
    return properties_[key];
}

template <typename WRITE_ITERATOR, typename OWNER>
int WriterDispatchingIterator<WRITE_ITERATOR, OWNER>::close() {
    // LOG_DEBUG_LIB(LibOdc) << "WriterDispatchingIterator<WRITE_ITERATOR>::close()" << std::endl;
    int rc = 0;
    for (typename Iterators::iterator it = iterators_.begin(); it != iterators_.end(); ++it) {
        rc |= (*it)->close();
        delete *it;
    }
    iterators_.clear();

    return rc;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace odc
