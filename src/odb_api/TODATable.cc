/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include "odb_api/odb_api.h"

//#include "odb_api/ODAColumn.h"
//#include "odb_api/SQLDatabase.h"
//#include "odb_api/SQLType.h"
//#include "odb_api/TODATable.h"

#include "odb_api/SQLBitfield.h"




namespace odb {
namespace sql {

template <typename T>
TODATable<T>::TODATable(SQLDatabase& owner, const eckit::PathName& path, const std::string& name):
	SQLTable(owner, path, name),
	data_(0),
	oda_(path),
	reader_(oda_.begin()),
    end_(oda_.end())
{
	populateMetaData();
}

template <typename T>
TODATable<T>::~TODATable() { delete[] data_; }

static const eckit::PathName nullPathName("<>");
static const std::string inputTable("input");

template <typename T>
TODATable<T>::TODATable(SQLDatabase& owner, eckit::DataHandle &dh):
	SQLTable(owner, nullPathName, inputTable),
	data_(0),
	oda_(dh),
	reader_(oda_.begin()),
    end_(oda_.end())
{
	populateMetaData();
}

template <typename T>
TODATable<T>::TODATable(SQLDatabase& owner, std::istream &is, const std::string &delimiter):
	SQLTable(owner, nullPathName, inputTable),
	data_(0),
	oda_(is, delimiter),
	reader_(oda_.begin()),
    end_(oda_.end())
{
	populateMetaData();
}

template <typename T>
void TODATable<T>::populateMetaData()
{
    using eckit::Log;
    
	Log::debug() << "TODATable::populateMetaData:" << std::endl;
	size_t count = reader_->columns().size();

	delete[] data_;
	data_ = new double[count];
	ASSERT(data_);

	for(size_t i = 0; i < count; i++)
	{
		Column& column = *reader_->columns()[i];

		const std::string name = column.name();
		bool hasMissing = column.hasMissing();
		double missing = column.missingValue();
		BitfieldDef bitfieldDef = column.bitfieldDef();
	
		std::string sqlType;
		switch(column.type())
		{
			case INTEGER: sqlType = "integer"; break;
			case STRING:  sqlType = "std::string"; break;
			case REAL:    sqlType = "real"; break;
			case DOUBLE:  sqlType = "double"; break;
			case BITFIELD:
				{
					std::string typeSignature = type::SQLBitfield::make("Bitfield", bitfieldDef.first, bitfieldDef.second, "DummyTypeAlias");
					addColumn(name, i, type::SQLType::lookup(typeSignature), hasMissing, missing, true, bitfieldDef);
					continue;
				}
				break;
			default:
				ASSERT("Unknown type" && 1==0);
				break;
		}
		SQLColumn *c = column.type() == BITFIELD
                        ? new ODAColumn(type::SQLType::lookup(sqlType), *this, name, i, hasMissing, missing, bitfieldDef, &data_[i])
                        : new ODAColumn(type::SQLType::lookup(sqlType), *this, name, i, hasMissing, missing, &data_[i]);
		addColumn(c, name, i);
	}
}

template <typename T>
void TODATable<T>::updateMetaData(const std::vector<SQLColumn*>& selected)
{
    using eckit::Log;

    Log::debug() << "ODATableIterator::updateMetaData: " << std::endl;
	MetaData newColumns (reader_->columns());
	for(size_t i = 0; i < selected.size(); i++)
	{
		ODAColumn *c = dynamic_cast<ODAColumn *>(selected[i]);
		ASSERT(c);
		if (newColumns.size() <= c->index() || newColumns[c->index()]->name() != c->name()) 
		{
			Log::warning() << "Column '" << c->fullName() << "': index has changed in new dataset." << std::endl;
			Log::warning() << "Was: " << c->index() << "." << std::endl;
			bool newIndexFound = false;
			for (size_t j = 0; j < newColumns.size(); ++j)
			{
				Column &other(*newColumns[j]);
				if (other.name() == c->name() || other.name() == c->fullName())
				{
					newIndexFound = true;
					Log::warning() << "New index: " << j << std::endl;
					c->index(j);
					break;
				}
			}
			ASSERT("One of selected columns does not exist in new data set." && newIndexFound);
		}
		//c->value(&data_[i]);
	}
}

template <typename T>
SQLColumn* TODATable<T>::createSQLColumn(const type::SQLType& type, const std::string& name, int index, bool hasMissingValue, double
missingValue)
{
	return new ODAColumn(type, *this, name, index, hasMissingValue, missingValue, &data_[index]);
}

template <typename T>
SQLColumn* TODATable<T>::createSQLColumn(const type::SQLType& type, const std::string& name, int index, bool hasMissingValue, double
missingValue, const BitfieldDef& bitfieldDef)
{
	return new ODAColumn(type, *this, name, index, hasMissingValue, missingValue, bitfieldDef, &data_[index]);
}


template <typename T>
bool TODATable<T>::hasColumn(const std::string& name, std::string* fullName)
{
    using eckit::Log;

    if (SQLTable::hasColumn(name))
	{
		if (fullName)
		{
			*fullName = name;
			Log::debug() << "TODATable<T>::hasColumn: name='" << name << "', fullName='" << *fullName << "'" << std::endl;
		}
		return true;
	}

	std::string colName = name + "@";

	int n = 0;
	std::map<std::string,SQLColumn*>::iterator it = columnsByName_.begin();
	for ( ; it != columnsByName_.end(); ++it)
	{
		std::string s = it->first;
		if (s.find(colName) == 0)
		{
			n++;
			if (fullName)
			{
				*fullName = s;
				Log::debug() << "TODATable<T>::hasColumn: colName='" << colName << "', fullName='" << *fullName << "'" << std::endl;
			}
		}
	}

	if (n == 0) return false;
	if (n == 1) return true;

	throw eckit::UserError(std::string("TODATable:hasColumn(\"") + name + "\"): ambiguous name");

	return false;
}

template <typename T>
SQLColumn* TODATable<T>::column(const std::string& name)
{
	std::string colName = name + "@";

	SQLColumn * column = 0;
	std::map<std::string,SQLColumn*>::iterator it = columnsByName_.begin();
	for ( ; it != columnsByName_.end(); ++it)
	{
		std::string s = it->first;
		if (s.find(colName) == 0)
		{
			if (column)
				throw eckit::UserError(std::string("TODATable::column: \"") + name + "\": ambiguous name.");
			else 
				column = it->second;
		}
	}
	if (column) return column;

	return SQLTable::column(name);
}

template <typename T>
SQLTableIterator* TODATable<T>::iterator(const std::vector<SQLColumn*>& x) const
{
	return new TableIterator(const_cast<TODATable&>(*this), reader_, end_, const_cast<double *>(data_), x);
}

} // namespace sql
} // namespace odb
