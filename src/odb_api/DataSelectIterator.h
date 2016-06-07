/// @file   DataSelectIterator.h
/// @author Tomas Kral

#ifndef ODBLIB_DATASELECTITERATOR_H_
#define ODBLIB_DATASELECTITERATOR_H_

#include "odb_api/DataRow.h"
#include "odb_api/IteratorFacade.h"
#include "ecml/core/ExecutionContext.h"

namespace odb
{
    namespace sql
    {
        class SQLSession;
        class SQLSelect;

        namespace expression { class Expressions; }
    }
}

namespace odb {

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
    double* const data() { return row_.data(); }
private:
    DataSelectIterator(const DataSelect& select, bool begin, ecml::ExecutionContext*);
    // DataSelectIterator(const DataSelect& select);

    DataSelectIterator(const DataSelectIterator&);
    DataSelectIterator& operator=(const DataSelectIterator&);

    DataSelectSession* createSession();
    void prepare();
    const odb::sql::expression::Expressions& results() const;

    const DataRow& dereference() const { return row_; }
    void increment();
    bool equal(const DataSelectIterator&) const;

    ecml::ExecutionContext* context_;
    const DataSelect& query_;
    DataSelectSession* session_;
    odb::sql::SQLSelect* select_;
    DataRow row_;
    bool aggregateResult_;
    bool noMore_;

    friend class odb::DataSelect;
    friend class odb::IteratorFacadeAccess;
};

} // namespace internal
} // namespace odb

#endif // ODBLIB_DATASELECTITERATOR_H_
