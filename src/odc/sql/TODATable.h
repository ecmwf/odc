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

#ifndef TODATable_H
#define TODATable_H

#include "eckit/sql/SQLTable.h"

#include "odc/Reader.h"
#include "odc/csv/TextReader.h"


namespace odc {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

template <typename READER>
class TODATable : public eckit::sql::SQLTable {
public:

    TODATable(eckit::sql::SQLDatabase& owner, const std::string& path, const std::string& name);

    virtual ~TODATable();

    const READER& oda() const;

private:  // methods

    void populateMetaData();
    //    void updateMetaData(const std::vector<SQLColumn*>&);

protected:  // methods

    TODATable(eckit::sql::SQLDatabase& owner, const std::string& path, const std::string& name, READER&& oda);

private:  // methods (overrides)

    virtual bool hasColumn(const std::string&) const override;
    virtual const eckit::sql::SQLColumn& column(const std::string&) const override;

    virtual eckit::sql::SQLTableIterator* iterator(
        const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>&,
        std::function<void(eckit::sql::SQLTableIterator&)> metadataUpdateCallback) const override;

    virtual void print(std::ostream& s) const override;

public:

    READER oda_;

private:  // members

    // This is a hack. Avoid calling begin() twice on non-seekable DataHandle if possible
    typename READER::iterator readerIterator_;
};

//----------------------------------------------------------------------------------------------------------------------

// Specific cases for Reader and TextReader

extern template class TODATable<Reader>;
extern template class TODATable<TextReader>;

struct ODATable : public TODATable<Reader> {
    ODATable(eckit::sql::SQLDatabase& owner, const std::string& path, const std::string& name) :
        TODATable<Reader>(owner, path, name, Reader(path)) {}
    ODATable(eckit::sql::SQLDatabase& owner, eckit::DataHandle& dh) :
        TODATable<Reader>(owner, "<>", "input", Reader(dh)) {}
};


struct ODBCSVTable : public TODATable<TextReader> {
    ODBCSVTable(eckit::sql::SQLDatabase& owner, const std::string& path, const std::string& name,
                const std::string& delimiter) :
        TODATable<TextReader>(owner, path, name, TextReader(path, delimiter)) {}
    ODBCSVTable(eckit::sql::SQLDatabase& owner, std::istream& is, const std::string& name,
                const std::string& delimiter) :
        TODATable<TextReader>(owner, "<none>", name, TextReader(is, delimiter)) {}
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace odc

#endif
