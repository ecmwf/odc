/// @file   DataRow.cc
/// @author Tomas Kral

#include "eckit/exception/Exceptions.h"
#include "odb_api/odblib/DataColumns.h"
#include "odb_api/odblib/DataRow.h"

using namespace std;

namespace odb {
namespace internal {

void DataRowBase::initialize(const DataColumns& columns)
{
    double* d = data();

    for (size_t i = 0; i < columns.size(); ++i, ++d)
    {
        *d = columns[i].missingValue();
    }
}

} // namespace internal

DataRow::DataRow(size_t n)
  : internal::DataRowBase(new double [n+1])
{
    ASSERT(data_);
    flag(STANDALONE);
    size(n);
}

DataRow::DataRow(size_t n, double value)
  : internal::DataRowBase(new double [n+1])
{
    ASSERT(data_);
    size(n);
    fill(begin(), end(), value);
    flag(STANDALONE | INITIALIZED);
}

DataRow::DataRow(const DataColumns& columns, bool init)
  : internal::DataRowBase(new double [columns.size()+1])
{
    ASSERT(data_);

    size(columns.size());
    flag(STANDALONE);

    if (init)
    {
        initialized(true);
        initialize(columns);
    }
}

DataRow::~DataRow()
{
    if (standalone())
        delete [] data_;
}

DataRow::DataRow(const DataRow& other)
  : internal::DataRowBase(new double [other.size() + 1])
{
    ASSERT(data_);
    size(other.size());
    copy(other.begin(), other.end(), begin());
    flag(STANDALONE | INITIALIZED);
}

// Template specializations.

template <>
std::string DataRow::get(size_t index) const
{
    const char *s = reinterpret_cast<const char*>(&at(index));

    size_t n = 0;
    for (; n < sizeof(double) && s[n]; ++n); 

    return std::string(s, n);
}

template <>
void DataRow::set(size_t index, const std::string& value)
{
    modified(true);

    const char* v = value.c_str();
    char* d = reinterpret_cast<char*>(&at(index));

    size_t j = 0;
    for (; j < sizeof(double) && v[j]; ++j) d[j] = v[j];
    for (; j < sizeof(double); ++j) d[j] = ' ';
}

} // namespace odb
