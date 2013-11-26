/// @file   DataTable.cc
/// @author Tomas Kral

#include "odblib/DataTable.h"
#include "odblib/DataPage.h"

#include <iostream>

using namespace std;

namespace odb {

using namespace internal;

DataTableProperties::DataTableProperties()
  : blockSizeInKb_(DEFAULT_BLOCK_SIZE_IN_KB),
    blockSizeInNumberOfRows_(DEFAULT_BLOCK_SIZE_IN_NUMBER_OF_ROWS),
    blockFillMark_(DEFAULT_BLOCK_FILL_MARK),
    blockFillMarkInNumberOfRows_(DEFAULT_BLOCK_FILL_MARK_IN_NUMBER_OF_ROWS)
{}

DataTable::DataTable(const string& name, const DataColumns& columns,
        const DataTableProperties& properties)
  : owner_(0),
    name_(name),
    columns_(columns),
    properties_(properties),
    frontPage_(0),
    backPage_(0),
    endPage_(0)
{
    frontPage_ = backPage_ = endPage_ = new DataPage(*this);
    ASSERT(frontPage_);
    frontPage_->rank(1);
}

DataTable::~DataTable()
{
    DataPage* page = frontPage_;

    while (page)
    {
        DataPage* next = page->next();
        delete page;
        page = next;
    }
}

size_t DataTable::size() const
{
    size_t size = 0;
    const DataPage* page = frontPage_;

    while (page)
    {
        size += page->size();

        if (page == backPage_)
            break;

        page = page->next();
    }

    return size;
}

bool DataTable::empty() const
{
    const DataPage* page = frontPage_;

    while (page)
    {
        if (!page->empty())
            return false;

        if (page == backPage_)
            break;

        page = page->next();
    }

    return true;
}

void DataTable::clear()
{
    DataPage* page = frontPage_;

    while (page)
    {
        page->clear();

        if (page == backPage_)
            break;

        page = page->next();
    }

    backPage_ = frontPage_;
}

DataRow& DataTable::operator[](size_t n)
{
    DataPage* page = frontPage_;

    while (page)
    {
        if (n < page->size())
            break;

        n -= page->size();
        page = page->next();
    }

    ASSERT(page && "Row index out of bounds!");

    return static_cast<DataRow&>(page->at(n));
}

size_t DataTable::capacity() const
{
    const DataPage* page = frontPage_;
    size_t c = 0;

    while (page)
    {
        c += page->capacity();
        page = page->next();
    }

    return c;
}

void DataTable::reserve(size_t n)
{
    size_t c = capacity();

    while (c < n)
    {
        DataPage* page = extend();
        c += page->capacity();
    }
}

void DataTable::resize(size_t n, bool initialize)
{
    size_t size_ = size();

    if (n > size_)
    {
        size_t more = n - size_ + backPage_->size();
        more -= backPage_->resize(more, initialize);

        while (more)
        {
            if (backPage_ == endPage_)
                backPage_ = extend();
            else
                backPage_ = backPage_->next();

            more -= backPage_->resize(more, initialize);
        }
    }
    else if (n < size_)
    {
        while ((size_ - backPage_->size()) >= n)
        {
            size_ -= backPage_->size();
            backPage_->resize(0);
            backPage_ = backPage_->previous();
            ASSERT(backPage_);
        }

        size_t s = n - (size_ - backPage_->size());
        ASSERT(s == backPage_->resize(s));
    }

    size_ = n;
}

void DataTable::push_back(const DataRow& r)
{
    // TODO: tidy this
    const DataRowProxy& row = reinterpret_cast<const DataRowProxy&>(r);

    if (!backPage_->push_back(row))
    {
        if (backPage_ != endPage_)
            backPage_ = backPage_->next(); 
        else
            backPage_ = extend();

        ASSERT(backPage_->push_back(row));
    }
}

void DataTable::push_back(const double* const data)
{
    if (!backPage_->push_back(data))
    {
        if (backPage_ != endPage_)
            backPage_ = backPage_->next(); 
        else
            backPage_ = extend();

        ASSERT(backPage_->push_back(data));
    }
}

DataTable::iterator DataTable::insert(iterator it, const DataRow& row)
{
    DataPage* first = it.block_;
    DataRowProxy* position = it.row_;

    if (first->insert(position, row))
        return it;

    DataPage* second = first->split();

    // Update links to the previous and the next page.
    second->previous(first);
    second->next(first->next());
    first->next(second);

    // Update ranks of the pages.
    DataPage* p = second;
    while (p)
    {
        p->rank(p->previous()->rank() + 1);
        p = p->next();
    }
    
    if (position <= first->end())
    {
        ASSERT(first->insert(position, row));
        return it;
    }
    else
    {
        position = second->begin() + (position - first->end());
        ASSERT(second->insert(position, row));
        return iterator(second, position);
    }
}

DataRow& DataTable::back()
{
    ASSERT(backPage_->size() > 0 && "No rows in the DataTable");
    return static_cast<DataRow&>(backPage_->back());
}

const DataRow& DataTable::back() const
{
    ASSERT(backPage_->size() > 0 && "No rows in the DataTable");
    return static_cast<const DataRow&>(backPage_->back());
}

DataPage* DataTable::extend()
{
    DataPage* page = new DataPage(*this);

    ASSERT(page);

    endPage_->next(page);
    page->rank(endPage_->rank() + 1);
    page->previous(endPage_);
    endPage_ = page;

    return page;
}

std::ostream& operator<<(std::ostream& os, DataTable& table)
{
    os << "DataTable(name=\"" << table.name() << "\", ";
    os << "size=" << table.size() << ")" << std::endl;

    return os;
}

} // namespace odb
