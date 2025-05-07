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
/// ECMWF Oct 2010

#ifndef odc_sql_TODATableIterator_H
#define odc_sql_TODATableIterator_H

#include "eckit/sql/SQLTable.h"

#include "odc/Reader.h"
#include "odc/csv/TextReader.h"


namespace odc {
namespace sql {

template <typename READER>
class TODATable;

//----------------------------------------------------------------------------------------------------------------------

template <typename READER>
class TODATableIterator : public eckit::sql::SQLTableIterator {

public:  // methods

    TODATableIterator(const TODATable<READER>& parent,
                      const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns,
                      std::function<void(eckit::sql::SQLTableIterator&)> metadataUpdateCallback,
                      const typename READER::iterator& seedIterator);
    virtual ~TODATableIterator();

private:  // methods (override>

    virtual void rewind() override;
    virtual bool next() override;

    virtual std::vector<size_t> columnOffsets() const override;
    virtual std::vector<size_t> doublesDataSizes() const override;
    virtual std::vector<char> columnsHaveMissing() const override;
    virtual std::vector<double> missingValues() const override;
    virtual const double* data() const override;

private:  // methods

    void updateMetaData();

private:  // members

    const TODATable<READER>& parent_;
    typename READER::iterator it_;
    typename READER::iterator end_;

    const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns_;
    std::vector<size_t> columnOffsets_;
    std::vector<size_t> columnDoublesSizes_;
    std::vector<char> columnsHaveMissing_;
    std::vector<double> columnMissingValues_;

    std::function<void(eckit::sql::SQLTableIterator&)> metadataUpdateCallback_;

    bool firstRow_;
};

extern template class TODATableIterator<Reader>;
extern template class TODATableIterator<TextReader>;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace odc

#endif
