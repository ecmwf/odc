/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "eckit/io/FileHandle.h"
#include "eckit/sql/SQLColumn.h"
#include "eckit/sql/SQLTableFactory.h"
#include "eckit/sql/type/SQLBitfield.h"
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Translator.h"

#include "odc/Reader.h"
#include "odc/csv/TextReader.h"
#include "odc/csv/TextReaderIterator.h"
#include "odc/sql/TODATable.h"
#include "odc/sql/TODATableIterator.h"

using namespace eckit;
using namespace eckit::sql;
using namespace odc::api;
using namespace odc::core;

namespace odc {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

namespace {

// Provide a factory such that when a table is specified in a from statement, the SQLParser
// can construct an appropriate table!

class ODAFactory : public eckit::sql::SQLTableFactoryBase {
    virtual SQLTable* build(SQLDatabase& owner, const std::string& name, const std::string& location) const override {

        PathName path(location);
        if (!path.exists())
            return 0;

        // Check that this is an ODB file
        FileHandle fh(path, false);
        fh.openForRead();
        AutoClose closer(fh);

        char buf[5];
        char oda[5]{'\xff', '\xff', 'O', 'D', 'A'};
        if (fh.read(buf, 5) != 5 || ::memcmp(buf, oda, 5) != 0)
            return 0;

        return new odc::sql::ODATable(owner, location, name);
    }
};

ODAFactory odaFactoryInstance;

}  // namespace

//---------------------------------------------------------------------------------------------------------------------


template <typename READER>
TODATable<READER>::TODATable(SQLDatabase& owner, const std::string& path, const std::string& name, READER&& oda) :
    SQLTable(owner, path, name), oda_(std::move(oda)), readerIterator_(oda_.begin()) {

    populateMetaData();
}


template <typename READER>
TODATable<READER>::~TODATable() {}

template <typename READER>
const READER& TODATable<READER>::oda() const {
    return oda_;
}


template <typename READER>
void TODATable<READER>::populateMetaData() {
    auto& it(readerIterator_);

    size_t count = it->columns().size();

    for (size_t i = 0; i < count; i++) {
        Column& column(*it->columns()[i]);

        const std::string name(column.name());
        bool hasMissing(column.hasMissing());
        double missing(column.missingValue());
        BitfieldDef bitfieldDef(column.bitfieldDef());

        std::string sqlType;
        size_t typeSizeDoubles = it->dataSizeDoubles(i);

        switch (column.type()) {
            case INTEGER:
                sqlType = "integer";
                break;
            case STRING:
                sqlType = "string";
                break;
            case REAL:
                sqlType = "real";
                break;
            case DOUBLE:
                sqlType = "double";
                break;
            case BITFIELD: {
                std::string typeSignature =
                    type::SQLBitfield::make("Bitfield", bitfieldDef.first, bitfieldDef.second, "DummyTypeAlias");
                addColumn(name, i, type::SQLType::lookup(typeSignature), hasMissing, missing, true, bitfieldDef);
                continue;
            }
            default:
                throw SeriousBug("Unknown type: " + Translator<int, std::string>()(column.type()), Here());
        }

        addColumn(name, i, type::SQLType::lookup(sqlType, typeSizeDoubles), hasMissing, missing,
                  column.type() == BITFIELD, bitfieldDef);
    }
}

// void TODATable<READER>::updateMetaData(const std::vector<SQLColumn*>& selected)
//{
//     // TODO: Whoah! whoah! whoah!
//     // n.b. we don't really want to modify the table. We should probabyl deal with this in the iterator...
//     NOTIMP;
//
////	MetaData newColumns (it_->columns());
////	for(size_t i = 0; i < selected.size(); i++)
////	{
////		ODAColumn *c = dynamic_cast<ODAColumn *>(selected[i]);
////		ASSERT(c);
////		if (newColumns.size() <= c->index() || newColumns[c->index()]->name() != c->name())
////		{
////			Log::warning() << "Column '" << c->fullName() << "': index has changed in new dataset." << endl
////			               << "Was: " << c->index() << "." << endl;
////			bool newIndexFound = false;
////			for (size_t j = 0; j < newColumns.size(); ++j)
////			{
////				Column &other(*newColumns[j]);
////				if (other.name() == c->name() || other.name() == c->fullName())
////				{
////					newIndexFound = true;
////					Log::warning() << "New index: " << j << endl;
////					c->index(j);
////					break;
////				}
////			}
////            if (! newIndexFound)
////            {
////                // TODO: if user specified MAYBE keyword, then use a constant NULL column.
////                //if (maybe_) {
////                //    Log::warning() << "Column '" << c->name() << "' not found." << endl;
////                //    selected[i] = new NullColumn(*selected[i]);
////                //} else {
////                    stringstream ss;
////                    ss << "One of selected columns, '" << c->name() << "', does not exist in new data set.";
////                    throw UserError(ss.str());
////                //}
////            }
////		}
////		//c->value(&data_[i]);
////	}
//}


template <typename READER>
bool TODATable<READER>::hasColumn(const std::string& name) const {

    // If the column is simply in the table, then use it.

    if (SQLTable::hasColumn(name)) {
        return true;
    }

    // Find columns that also have an (unspecified) section name

    std::string colName(name + "@");
    int n = 0;

    for (const auto& column : columnsByName_) {
        const std::string& s(column.first);
        if (StringTools::startsWith(s, colName)) {
            n++;
        }
    }

    if (n == 1)
        return true;
    if (n > 1) {
        throw UserError(std::string("TODATable:hasColumn(\"") + name + "\"): ambiguous name");
    }

    return false;
}

template <typename READER>
const SQLColumn& TODATable<READER>::column(const std::string& name) const {

    // If the column is simply in the table, then use it.

    if (SQLTable::hasColumn(name)) {
        return SQLTable::column(name);
    }

    // Find columns that also have an (unspecified) section name

    const std::string colName(name + "@");
    SQLColumn* column = 0;

    for (const auto& col : columnsByName_) {
        const std::string& s(col.first);
        if (StringTools::startsWith(s, colName)) {
            if (column)
                throw UserError(std::string("TODATable:hasColumn(\"") + name + "\"): ambiguous name");
            column = col.second;
        }
    }

    if (!column)
        throw SeriousBug("Requesting column \"" + name + "\": not found", Here());

    return *column;
}

template <typename READER>
SQLTableIterator* TODATable<READER>::iterator(
    const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns,
    std::function<void(eckit::sql::SQLTableIterator&)> metadataUpdateCallback) const {
    return new TODATableIterator<READER>(*this, columns, metadataUpdateCallback, readerIterator_);
}

template <typename READER>
void TODATable<READER>::print(std::ostream& s) const {
    s << "TODATable(" << path_ << ")";
}


// Explicit instantiation

template class TODATable<Reader>;
template class TODATable<TextReader>;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace odc
