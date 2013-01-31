/// @file   DataTableFiller.h
/// @author Tomas Kral

#ifndef DATATABLEFILLER_H_
#define DATATABLEFILLER_H_

#include <vector>
#include <iterator>

namespace odb {

class DataTable;

namespace internal {

class DataTableFillerIterator;

/*! Fills DataTable with rows from data source.
 *
 *  The DataTableFiller class is responsible for filling up a DataTable with
 *  rows from data source (i.e. ODB file).
 *
 *  @ingroup data
 */
class DataTableFiller
{
public:
    typedef DataTableFillerIterator iterator;

    explicit DataTableFiller(DataTable& table);
    ~DataTableFiller();

    iterator begin();

    void addColumn(size_t sourceColumnIndex);
    void addLink(size_t linkLenIndex);

private:
    DataTableFiller(const DataTableFiller&);
    DataTableFiller& operator=(const DataTableFiller&);

    DataTable& targetTable_;
    DataTableFiller* childFiller_;
    std::vector<size_t> sourceColumnIndexes_;
    std::vector<size_t> linkLenIndexes_;
    double* rowBuffer_;
    size_t rowBufferSize_;

    friend class DataTableFillerIterator;
};

class DataTableFillerIterator
  : public std::iterator<std::output_iterator_tag, DataTableFillerIterator>
{
public:
    explicit DataTableFillerIterator(DataTableFiller& owner);
    ~DataTableFillerIterator();

    DataTableFillerIterator& operator*() { return *this; }
    DataTableFillerIterator& operator=(const double* data);
    DataTableFillerIterator& operator++();

private:
    void updateRowsToSkip(const double* data);
    void fillRow(const double* const data);

private:
    DataTableFiller* owner_;
    DataTable* targetTable_;
    std::vector<size_t> sourceColumnIndexes_;
    std::vector<size_t> linkLenIndexes_;
    double* rowBuffer_;
    size_t rowsToSkip_;
    bool sourceColumnsAligned_;
};

} // namespace internal
} // namespace odb

#endif // DATATABLEFILLER_H_
