/// @file   DataLoader.cc
/// @author Tomas Kral

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/FileHandle.h"
#include "odblib/DataColumns.h"
#include "odblib/DataLoader.h"
#include "odblib/DataSetBuilder.h"
#include "odblib/DataSetFiller.h"
#include "odblib/DataSet.h"
#include "odblib/DataTable.h"
#include "odblib/Select.h"
#include "odblib/Reader.h"
#include "odblib/SelectIterator.h"
#include "odblib/TextReader.h"
#include "odblib/TextReaderIterator.h"

using namespace std;
using namespace eckit;

namespace odb {

DataLoader::DataLoader(const std::string& path)
  : query_(""),
    path_(path),
    handle_(0),
    deleteHandle_(true),
    stream_(0),
    deleteStream_(true),
    mappings_(),
    open_(true)
{
    open(path);
}

DataLoader::DataLoader(DataHandle& handle)
  : query_(""),
    path_(""),
    handle_(&handle),
    deleteHandle_(false),
    stream_(0),
    deleteStream_(true),
    mappings_(),
    open_(true)
{}

DataLoader::DataLoader(std::istream& stream)
  : query_(""),
    path_(""),
    handle_(0),
    deleteHandle_(true),
    stream_(&stream),
    deleteStream_(false),
    mappings_(),
    open_(true)
{}

DataLoader::~DataLoader()
{
    close();
}

void DataLoader::open(const std::string& path)
{
    std::string ext = path.substr(path.size()-4);

    if (ext == ".csv" || ext == ".txt")
    {
        stream_ = new std::ifstream(path.c_str());
        ASSERT(stream_);
    }
    else {
        handle_ = new FileHandle(path);
        ASSERT(handle_);
        handle_->openForRead();
    }

    open_ = true;
}

void DataLoader::select(const std::string& query)
{
    query_ = query;

    if (handle_)
        handle_->rewind();
    else if (stream_)
        stream_->seekg(0);
}

void DataLoader::fill(DataSet& dataset)
{
    ASSERT(open_);

    if (handle_)
        fillFromBinary(dataset);
    else if (stream_)
        fillFromText(dataset);
    else
        throw eckit::UserError("DataLoader: Unrecognized data source: " + path_);
}

void DataLoader::fill(DataSet& dataset, const std::string& tableName)
{
    ASSERT(!query_.empty());
    fillFromQuery(dataset, tableName);
}

void DataLoader::fillFromText(DataSet& dataset)
{
    using namespace internal;

    ASSERT(stream_);
    ASSERT(query_.empty());

    TextReader reader(*stream_, ","); // FIXME: delimiter is hardcoded now
    TextReader::iterator source = reader.begin();
    MetaData& metadata = source->columns();

    DataSetBuilder builder(metadata, mappings_, true);
    builder.build(dataset);

    DataSetFiller filler(dataset, metadata);
    DataSetFiller::iterator target = filler.begin();

    for (; source != reader.end(); ++source, ++target)
        *target = source->data();
}

void DataLoader::fillFromBinary(DataSet& dataset)
{
    using namespace internal;

    ASSERT(handle_);
    ASSERT(query_.empty());

    Reader reader(*handle_);
    Reader::iterator source = reader.begin();
    MetaData& metadata = source->columns();

    DataSetBuilder builder(metadata, mappings_, true);
    builder.build(dataset);

    DataSetFiller filler(dataset, metadata);
    DataSetFiller::iterator target = filler.begin();

    for (; source != reader.end(); ++source, ++target)
        *target = source->data();
}

void DataLoader::fillFromQuery(DataSet& dataset, const std::string& name) const
{
    Select select(query_, *handle_);
    Select::iterator source = select.begin();

    DataColumns columns = source->columns();
    DataTable* table = 0;

    if (dataset.tables().count(name))
    {
        table = *dataset.tables().find(name);
        ASSERT(table);
        // TODO: Maybe compare not only the number of columns but also
        // the column name and data type.
        ASSERT(table->columns().size() == columns.size());
    }
    else
    {
        table = new DataTable(name, columns);
        ASSERT(table);
        dataset.tables().insert(table);
    }

    for (; source != select.end(); ++source)
        table->push_back(source->data());
}

void DataLoader::close()
{
    if (deleteHandle_) delete handle_;
    if (deleteStream_) delete stream_;

    deleteHandle_ = true;
    deleteStream_ = true;
    query_.clear();
    open_ = false;
}

} // namespace odb
