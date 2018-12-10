/// @file   DataJoinIterator.h
/// @author Tomas Kral

#ifndef DATAJOINITERATOR_H_
#define DATAJOINITERATOR_H_

#include "odb_api/DataRow.h"
#include "odb_api/DataTable.h"
#include "odb_api/IteratorFacade.h"

namespace odc {

class DataJoin;

namespace internal {

/*! @internal
 *  @brief Input iterator providing read-only access to joined tables.
 *  @see DataJoin
 *  @ingroup data
 */
class DataJoinIterator
  : public InputIteratorFacade<DataJoinIterator, const DataRow>
{
public:
    ~DataJoinIterator();

private:
    DataJoinIterator(const DataJoin& join, bool begin);
    DataJoinIterator(const DataJoinIterator&);
    DataJoinIterator& operator=(const DataJoinIterator&);

    const DataRow& dereference() const
    { return reinterpret_cast<const DataRow&>(rowProxy_); }

    void increment();

    bool equal(const DataJoinIterator& other) const
    { return done_; }

    void initialize();
    void populate();

private:
    const DataJoin& join_;
    size_t columnsSize_;
    size_t rowsSize_;
    size_t cacheSize_;
    double* const cache_;
    double* cacheIt_;
    double* cacheEnd_;
    DataRowProxy rowProxy_;
    DataTable::const_iterator leftIt_;
    DataTable::const_iterator leftEnd_;
    size_t leftSize_;
    DataTable::const_iterator rightIt_;
    DataTable::const_iterator rightEnd_;
    size_t rightSize_;
    size_t primaryKeyIndex_;
    size_t foreignKeyIndex_;
    int type_;
    bool done_;

    friend class odc::DataJoin;
    friend class odc::IteratorFacadeAccess;
};

} // namespace internal
} // namespace odc

#endif // DATAJOINITERATOR_H_
