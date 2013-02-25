/// @file   DataSetBuilder.cc
/// @author Tomas Kral

#include "odblib/DataSetBuilder.h"

#include <map>
#include <string>

#include "odblib/Column.h"
#include "odblib/MetaData.h"

#include "odblib/DataSet.h"
#include "odblib/DataTable.h"
#include "odblib/DataLink.h"
#include "odblib/DataColumn.h"

using namespace std;
using namespace eclib;

namespace odb {

namespace {

enum TokenType
{
    TABLE_COLUMN = 0,
    LINK_OFFSET_COLUMN = 1,
    LINK_LEN_COLUMN = 2
};

struct Token
{
    TokenType type;
    std::string columnName;
    odb::ColumnType columnType;
    std::string tableName;
    std::string parentTableName;
    std::string childTableName;
};

typedef std::vector<Token> TokenVector;
TokenVector tokenize(const odb::MetaData& metaData);

TokenVector tokenize(const odb::MetaData& metaData)
{
    TokenVector tokens;

    for (size_t i = 0; i < metaData.size(); i++)
    {
        size_t pos;
        Token token;
        odb::Column& column = *metaData[i];

        token.columnName = column.name();
        token.columnType = column.type();

        if ((pos = token.columnName.find(".offset@")) != string::npos)
        {
            token.type = LINK_OFFSET_COLUMN;
            token.childTableName = token.columnName.substr(0, pos);
            token.parentTableName = token.columnName.substr(pos + 8);
        }
        else if ((pos = token.columnName.find(".len@")) != string::npos)
        {
            token.type = LINK_LEN_COLUMN;
            token.childTableName = token.columnName.substr(0, pos);
            token.parentTableName = token.columnName.substr(pos + 5);
        }
        else
        {
            token.type = TABLE_COLUMN;
            pos = token.columnName.find("@");
            token.tableName = token.columnName.substr(pos + 1);

            if (token.tableName.empty())
                throw UserError("Column " + token.columnName
                       + "does not belong to any table.");
        }

        tokens.push_back(token);
    }

    return tokens;
}

ColumnType convertColumnType(odb::ColumnType type)
{
    switch (type)
    {
        case odb::INTEGER:  return INTEGER;
        case odb::REAL:     return REAL;
        case odb::DOUBLE:   return DOUBLE;
        case odb::STRING:   return STRING;
        case odb::BITFIELD: return BITFIELD;
        default: ASSERT(!"Unexpected odb::ColumnType.");
    };

    return ColumnType(0); // never reached
}

} // namespace

DataSetBuilder::DataSetBuilder(const odb::MetaData& metadata, bool buildLinks)
  : metadata_(metadata),
    mapping_(),
    buildLinks_(buildLinks)
{}

DataSetBuilder::DataSetBuilder(const odb::MetaData& metadata,
        const DataTableMappings& mapping, bool buildLinks)
  : metadata_(metadata),
    mapping_(mapping),
    buildLinks_(buildLinks)
{}

void DataSetBuilder::build(DataSet& dataset) const
{
    buildTables(dataset);

    if (buildLinks_)
        buildLinks(dataset);
}

void DataSetBuilder::buildTables(DataSet& dataset) const
{
    typedef map<string, DataColumns> ColumnsMap;

    ColumnsMap columnsMap;
    TokenVector tokens = tokenize(metadata_);

    for (size_t i = 0; i < tokens.size(); i++)
    {
        Token& token = tokens[i];

        if (token.type == TABLE_COLUMN)
        {
            string name = token.tableName;

            DataTableMappings::const_iterator it = mapping_.find(name);
            if (it != mapping_.end())
                name = it->second;

            DataColumns& columns = columnsMap[name];
            columns.push_back(DataColumn(*metadata_[i]));
        }
    }

    for (ColumnsMap::iterator it = columnsMap.begin();
            it != columnsMap.end(); ++it)
    {
        DataTable* table = new DataTable(it->first, it->second);
        dataset.tables().insert(table);
    }
}

void DataSetBuilder::buildLinks(DataSet& dataset) const
{
    TokenVector tokens = tokenize(metadata_);

    Token previousToken;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        Token& token = tokens[i];

        switch (token.type)
        {
        case (TABLE_COLUMN):
            break;
        case (LINK_OFFSET_COLUMN):
            // Nothing to be done here. We assume that offset columns
            // are always followed by len columns.
            break;

        case (LINK_LEN_COLUMN):
        {
            ASSERT(previousToken.type == LINK_OFFSET_COLUMN);
            ASSERT(previousToken.parentTableName == token.parentTableName);
            ASSERT(previousToken.childTableName == token.childTableName);

            string parentName = token.parentTableName;
            string childName = token.childTableName;

            // Look for table mappings.
            DataTableMappings::const_iterator it = mapping_.find(parentName);
            if (it != mapping_.end())
                parentName = it->second;

            it = mapping_.find(childName);
            if (it != mapping_.end())
                childName = it->second;

            // Ignore links between two aligned tables mapped into a
            // single one.
            if (parentName == childName)
            {
                Log::info() << "Ignoring link between aligned tables "
                    << token.parentTableName << " and " 
                    << token.childTableName << endl;
                break;
            }

            DataTables::iterator t;
            // Find parent table. If there is none issue a warning and skip
            // building the link.
            t = dataset.tables().find(parentName);
            if (t == dataset.tables().end())
            {
                Log::warning() << "No parent table found for the link "
                    << token.columnName << endl;
                break;
            }
            DataTable* parent = *t;

            // Find child table. If there is none issue a warning and skip
            // building the link.
            t = dataset.tables().find(childName);
            if (t == dataset.tables().end())
            {
                Log::warning() << "No child table found for the link "
                    << token.columnName << endl;
                break;
            }
            DataTable* child = *t;

            // Make sure that in case of multiple aligned tables
            // which are joined to a signle table using table mappings
            // we create only one link.
            DataLinks::const_iterator l;
            l = dataset.links().find(parent, child);
            if (l == dataset.links().end())
            {
                DataLink* link = new DataLink(*parent, *child);
                link->offsetName(previousToken.columnName);
                link->lengthName(token.columnName);
                dataset.links().insert(link);
            }

        }   break;
        }

        previousToken = token;
    }
}

} // namespace odb
