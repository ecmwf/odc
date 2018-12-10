/// @file   DataSelectIterator.h
/// @author Tomas Kral

#ifndef ODBLIB_DATASELECTITERATOR_H_
#define ODBLIB_DATASELECTITERATOR_H_

#include "odb_api/DataRow.h"
#include "odb_api/IteratorFacade.h"

namespace odc
{
    namespace sql
    {
        class SQLSession;
        class SQLSelect;

        namespace expression { class Expressions; }
    }
}

namespace odc {

class DataSelect;

namespace internal {

class DataSelectSession;

/*! @internal
 *  @brief Input iterator providing a read-only access to SQL query results.
 *  @ingroup data
 */
class DataSelectIterator
  : public InputIteratorFacade<DataSelectIterator, const DataRow>
{
public:
   ~DataSelectIterator();
    const double* data() const { return row_.data(); }
    double* data() { return row_.data(); }
private:
    DataSelectIterator(const DataSelect& select, bool begin);
    // DataSelectIterator(const DataSelect& select);

    DataSelectIterator(const DataSelectIterator&);
    DataSelectIterator& operator=(const DataSelectIterator&);

    DataSelectSession* createSession();
    void prepare();
    const odc::sql::expression::Expressions& results() const;

    const DataRow& dereference() const { return row_; }
    void increment();
    bool equal(const DataSelectIterator&) const;

    const DataSelect& query_;
    DataSelectSession* session_;
    odc::sql::SQLSelect* select_;
    DataRow row_;
    bool aggregateResult_;
    bool noMore_;

    friend class odc::DataSelect;
    friend class odc::IteratorFacadeAccess;
};

} // namespace internal
} // namespace odc

#endif // ODBLIB_DATASELECTITERATOR_H_
