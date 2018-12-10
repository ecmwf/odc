/// @file   DataPage.cc
/// @author Tomas Kral

#include "odb_api/DataPage.h"
#include "odb_api/DataTable.h"

using namespace std;

namespace odc {
namespace internal {

DataPage::DataPage(DataTable& table)
  : table_(table),
    width_(table.columns().size() + 1), // + 1 extra column for row flags
    height_(optimizeHeight(table)),
    fillMark_(optimizeFillMark(table)),
    size_(0),
    rowsAligned_(true),
    buffer_(new double [(width_ + 1) * height_]), // + 1 extra column for row pointers
    data_(buffer_ + height_),
    rows_(reinterpret_cast<DataRowProxy*>(buffer_)),
    begin_(rows_),
    previous_(0),
    next_(0),
    rank_(0)
{
    ASSERT(buffer_);
    alignRows(height_);
}

DataPage::~DataPage()
{
    delete [] buffer_;
}

size_t DataPage::resize(size_t n, bool init)
{
    // Do not resize the page beyond its fill mark.
    if (n > fillMark_)
        n = fillMark_;

    if (n < size_)
    {
        drop(begin() + n, end());
    }
    else if (n > size_)
    {
        DataRowProxy* row = end();

        ASSERT(!row->used());

        if (init)
        {
            row->initialize(table_.columns());
            row->flag(DataRow::USED | DataRow::INITIALIZED);
        }
        else
        {
            row->used(true);
        }

        for (DataRowProxy* r = row + 1; r != rows_ + n; ++r)
        {
            ASSERT(!r->used());

            if (init)
            {
                if (r->modified() || !r->initialized())
                    copy(row->data(), row->data() + width_ - 1, r->data());
                
                r->flag(DataRow::USED | DataRow::INITIALIZED);
            }
            else
            {
                r->used(true);
            }
        }
    }

    return (size_ = n);
}

void DataPage::clear()
{
    drop(begin(), end());

    if (!rowsAligned_)
    {
        alignRows(size_);
        rowsAligned_ = true;
    }

    size_ = 0;
}

bool DataPage::push_back(const DataRow& row)
{
    if (size_ >= fillMark_)
        return false;

    ++size_;

    copy(row.begin(), row.end(), back().begin());
    back().flag(DataRow::USED);

    return true;
}

bool DataPage::push_back(const double* const data)
{
    if (size_ >= fillMark_)
        return false;

    ++size_;

    copy(data, data + width_ - 1, back().data());
    back().flag(DataRow::USED);

    return true;
}

bool DataPage::insert(DataRowProxy* pos, const DataRow& row)
{
    ASSERT(row.size() == width_ - 1);
    ASSERT(begin() <= pos && pos <= end() && "DataPage::insert(): Invalid position.");

    if (full())
        return false;

    DataRowProxy last = *end();
    last.used(true);

    copy(row.begin(), row.end(), last.begin());
    copy_backward(pos, end(), end() + 1);
    *pos = last;

    if (pos != end())
        rowsAligned_ = false;

    ++size_;

    return true;
}

DataPage* DataPage::split()
{
    DataPage* page = new DataPage(table_);

    DataRowProxy* first = begin() + size_ / 2;
    for (DataRowProxy* row = first; row != end(); ++row)
    {
        page->push_back(row->data());
    }

    size_ /= 2;

    return page;
}

void DataPage::drop(DataRowProxy* begin, DataRowProxy* end)
{
    for (DataRowProxy* row = begin; row != end; ++row)
    {
        ASSERT(row->used());
        row->used(false);
    }
}

void DataPage::alignRows(size_t n)
{
    for (size_t i = 0; i < n; ++i)
    {
        rows_[i] = &data_[i * width_];
        rows_[i].flag(0);
    }
}

void DataPage::initializeRows(size_t n)
{
    if (n == 0) return;

    rows_->initialize(table_.columns());
    rows_->flag(DataRow::INITIALIZED);

    double* prev = data_;
    double* next = data_ + width_;
    double* last = data_ + width_ * n;

    for (; next != last; prev = next, next += width_)
    {
        std::copy(prev, prev + width_, next);
    }
}

size_t DataPage::optimizeHeight(const DataTable& table)
{
    const DataTableProperties& properties = table.properties();
    const DataColumns& columns = table.columns();

    size_t height = 0;

    if (properties.blockSizeInNumberOfRows())
        height = properties.blockSizeInNumberOfRows();
    else
    {
        size_t sizeInKb = properties.blockSizeInKb();
        height = sizeInKb * 1000 / sizeof(double) / (columns.size() + 1);
        height = max(height, (size_t) MIN_BLOCK_HEIGHT);
    }

    return height;
}

size_t DataPage::optimizeFillMark(const DataTable& table)
{
    const DataTableProperties& properties = table.properties();
    size_t fillMark = 0;

    if (properties.blockFillMarkInNumberOfRows())
    {
        fillMark = properties.blockFillMarkInNumberOfRows();
        ASSERT(properties.blockSizeInNumberOfRows() > fillMark);
    }
    else
    {
        double ratio = properties.blockFillMark();
        ASSERT(ratio > 0.0 && ratio <= 1.0);
        ASSERT(properties.blockSizeInKb() != 0.0);
        fillMark = optimizeHeight(table) * ratio;
    }

    return fillMark;
}

} // namespace internal
} // namespace odc
