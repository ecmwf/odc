/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odc_core_MetaData_H
#define odc_core_MetaData_H

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLTypedefs.h"
#include "odc/core/Column.h"

#ifdef SWIGPYTHON
#include "odc/IteratorProxy.h"
#endif

namespace eckit {
class PathName;
}

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

typedef std::vector<Column*> MetaDataBase;
using Properties = std::map<std::string, std::string>;

class MetaData : public MetaDataBase {
public:

    MetaData();
    MetaData(int);
    MetaData(int, Column*);
    MetaData(const MetaData&);
    MetaData* clone() const;

    unsigned long long rowsNumber() const { return rowsNumber_; }
    void rowsNumber(unsigned long long n) { rowsNumber_ = n; }

    unsigned long long dataSize() const { return dataSize_; }
    void dataSize(unsigned long long n) { dataSize_ = n; }

    MetaData& operator=(const MetaData&);
    MetaData& operator+=(const MetaData&);
    MetaData operator+(const MetaData&);

    /// Check if number of columns, column names and column types are equal. Values not checked.
    bool operator==(const MetaData&) const;
    bool equals(const MetaData& md, bool compareDataSizes = true) const;

    bool equalsIncludingConstants(const MetaData&, const std::vector<std::string>& constColumns) const;

    bool operator!=(const MetaData& other) const { return !(*this == other); }
    bool compatible(const MetaData& other) const;

    void operator|=(const MetaData& other);

    template <typename ByteStream>
    void save(DataStream<ByteStream>& ds) const;
    template <typename ByteStream>
    void load(DataStream<ByteStream>& ds);

    void setSize(size_t);

    MetaData& addColumn(const std::string& name, const std::string& type);

    template <typename ByteOrder>
    MetaData& addColumnPrivate(const std::string& name, const std::string& type);

    bool allColumnsInitialised() const;

    MetaData& addBitfield(const std::string& name, const eckit::sql::BitfieldDef&);
    template <typename ByteOrder>
    MetaData& addBitfieldPrivate(const std::string& name, const eckit::sql::BitfieldDef&);

    bool hasColumn(const std::string&) const;

    /// \brief Return a pointer to the object representing the column with the specified name.
    /// The name may, but does not need to, be qualified with a table name (e.g. both `varno` and
    /// `varno@body` are accepted).
    ///
    /// This function throws the same exceptions as columnIndex().
    Column* columnByName(const std::string&) const;

    /// \brief Return the index of the column with the specified name. The name may, but does not
    /// need to, be qualified with a table name (e.g. both `varno` and `varno@body` are accepted).
    ///
    /// If no column with this name is found, a ColumnNotFoundException is thrown.
    ///
    /// If the column name is not qualified with a table name and a column with this name is found
    /// in multiple tables, an AmbiguousColumnException is thrown.
    size_t columnIndex(const std::string&) const;

    static api::ColumnType convertType(const std::string&);
#ifdef SWIGPYTHON
    std::string __str__() {
        std::stringstream s;
        s << "[";
        for (size_t i = 0; i < size(); ++i) {
            s << at(i)->__repr__() << ",";
        }
        s << "]";
        return s.str();
    }
#endif

    void resetStats();

    template <typename ByteOrder>
    void resetCodecs() {
        for (auto& col : *this)
            col->resetCodec<ByteOrder>();
    }

    virtual ~MetaData();

    virtual void print(std::ostream& s) const;

    friend std::ostream& operator<<(std::ostream& s, const MetaData& p) {
        p.print(s);
        return s;
    }

private:

    unsigned long long rowsNumber_;
    unsigned long long dataSize_;
};


template <typename ByteOrder>
void MetaData::save(DataStream<ByteOrder>& ds) const {
    ds.write(static_cast<int32_t>(size()));
    for (size_t i = 0; i < size(); i++) {
        at(i)->save(ds);
    }
}

template <typename ByteOrder>
void MetaData::load(DataStream<ByteOrder>& ds) {
    for (size_t i = 0; i < size(); i++)
        delete at(i);
    clear();

    int32_t nCols;
    ds.read(nCols);
    resize(nCols, NULL);
    for (size_t i = 0; i < size(); i++) {
        delete at(i);
        at(i) = new Column(*this);
        at(i)->load(ds);
    }
}

template <typename ByteOrder>
MetaData& MetaData::addColumnPrivate(const std::string& name, const std::string& type) {
    Column* c = new Column(*this);
    ASSERT(c);

    c->name(name);
    c->type<ByteOrder>(Column::type(type));

    push_back(c);
    return *this;
}

template <typename ByteOrder>
MetaData& MetaData::addBitfieldPrivate(const std::string& name, const eckit::sql::BitfieldDef& bd) {
    Column* c = new Column(*this);
    ASSERT(c);
    c->name(name);
    c->type<ByteOrder>(api::BITFIELD);
    c->bitfieldDef(bd);
    push_back(c);

    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

/// Return true if `fullColumnName` is equal to `columnNamePossiblyWithoutTableName` or
/// if it starts with `columnNamePossiblyWithoutTableName` followed by the `@` character.
bool columnNameMatches(const std::string& fullColumnName, const std::string& columnNamePossiblyWithoutTableName);

//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc

#endif
