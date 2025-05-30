/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author Simon Smart
/// @date Feb 2009

#ifndef odc_SelectIterator_H
#define odc_SelectIterator_H

#include "eckit/sql/SQLSession.h"
#include "eckit/sql/expression/SQLExpressions.h"
#include "odc/api/ColumnType.h"
#include "odc/sql/SQLSelectOutput.h"


// Forward declarations

namespace odc {
class Select;
template <typename I, typename O, typename D>
class IteratorProxy;
namespace core {
class MetaData;
}
}  // namespace odc

namespace eckit {
namespace SQL {
class SQLSelect;
}
}  // namespace eckit


namespace odc {

//----------------------------------------------------------------------------------------------------------------------

class SelectIterator : private eckit::NonCopyable {
public:

    SelectIterator(const std::string& select, eckit::sql::SQLSession& session, sql::SQLSelectOutput& output);
    ~SelectIterator();

    // TODO: New dataset
    bool isNewDataset() { return output_.isNewDataset(); }
    const double* data() const { return output_.data(); }
    //    double* data() { return data_; }
    double& data(size_t i) { return output_.data(i); }

    const core::MetaData& columns() const { return output_.metadata(); }
    const core::MetaData& columns(const core::MetaData&) { NOTIMP; }
    void setNumberOfColumns(size_t) { NOTIMP; }

    const std::map<std::string, std::string>& properties() const { NOTIMP; }

    int close() { NOTIMP; }
    int setColumn(size_t index, std::string name, api::ColumnType type) { NOTIMP; }
    void writeHeader() { NOTIMP; }
    int setBitfieldColumn(size_t index, std::string name, api::ColumnType type, eckit::sql::BitfieldDef b) { NOTIMP; }
    void missingValue(size_t, double) { NOTIMP; }

    /// Set an output buffer for retrieving the next row(s)
    void setOutputRowBuffer(double* data, size_t count = 0);

    /// The offset of a given column in the doubles[] data array
    size_t dataOffset(size_t i) const { return output_.dataOffset(i); }

    // Get the number of doubles per row.
    size_t rowDataSizeDoubles() const { return output_.rowDataSizeDoubles(); }

    bool next();

private:

    void parse();

    std::string select_;

    sql::SQLSelectOutput& output_;

    eckit::sql::SQLSelect* selectStmt_;  // n.b. non-owning

    eckit::sql::SQLSession& session_;

    // Just for integration with IteratorProxy. Yeaurgh.
    // TODO: Remove this hack
    bool noMore_;

protected:

    int refCount_;

    // This is a bit yucky, but the IteratorProxy essentially reimplements std::shared_ptr
    // but not in a threadsafe way.
    friend class odc::IteratorProxy<odc::SelectIterator, odc::Select, const double>;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace odc

#endif
