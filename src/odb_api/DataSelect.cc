/// @file   DataSelect.cc
/// @author Tomas Kral

#include "odb_api/DataSelect.h"
#include "odb_api/DataSelectIterator.h"
#include "odb_api/DataSet.h"
#include "odb_api/DataTable.h"
#include "odb_api/Expressions.h"
#include "odb_api/sql/SQLDataSet.h"
#include "odb_api/sql/SQLDataTable.h"

using namespace odb;
using namespace eckit;

namespace odb {


DataSelect::DataSelect(const std::string& statement, const DataSet& dataset)
  : statement_(statement),
    dataset_(&dataset),
    table_(0),
    begin_(new internal::DataSelectIterator(*this, true))
{
    populateColumns();
}

DataSelect::DataSelect(const std::string& statement, const DataTable& table)
  : statement_(statement),
    dataset_(0),
    table_(&table),
    begin_(new internal::DataSelectIterator(*this, true))
{
    populateColumns();
}

sql::SQLDatabase* DataSelect::database() const
{
    sql::SQLDataSet* db = new sql::SQLDataSet(table_ ? table_->name() : dataset_->name());

    ASSERT(db);

    if (dataset_)
    {
        for (DataTables::const_iterator it = dataset_->tables().begin();
                it != dataset_->tables().end(); ++it)
        {
            SQLDataTable* table = new SQLDataTable(*db, **it);
            ASSERT(table);
            db->addTable(table);
        }
    }
    else if (table_)
    {
        SQLDataTable* table = new SQLDataTable(*db, *table_);
        ASSERT(table);
        db->addTable(table);
    }

    return db;
}

DataSelect::~DataSelect()
{}

DataSelect::iterator DataSelect::begin()
{
    internal::DataSelectIterator* it = begin_.get()
        ? begin_.release()
        : new internal::DataSelectIterator(*this, true);

    ASSERT(it);

    it->prepare();

    return iterator(it);
}

DataSelect::iterator DataSelect::end() const
{
    return iterator(new internal::DataSelectIterator(*this, false));
}

void DataSelect::populateColumns()
{
    ASSERT(begin_.get());

    const Expressions& results = begin_->results();

    for (size_t i = 0; i < results.size(); i++)
    {
        const sql::expression::SQLExpression* exp = results[i];
        const sql::type::SQLType* sqlType = exp->type();
        const int kind = sqlType->getKind();

        ColumnType dataType;

        switch (kind)
        {
            using namespace odb::sql::type;

            case SQLType::realType:    dataType = REAL;     break;
            case SQLType::doubleType:  dataType = DOUBLE;   break;
            case SQLType::integerType: dataType = INTEGER;  break;
            case SQLType::stringType:  dataType = STRING;   break;
            case SQLType::bitmapType:  dataType = BITFIELD; break;
            case SQLType::blobType: NOTIMP; break;
            default:
                Log::error() << "Unknown type: " << *sqlType << ", kind: " << kind << std::endl;
                ASSERT(!"UnknownType");
                break;
        }

        DataColumn c(exp->title(), dataType);
        // TODO: c.hasMissing(exp->hasMissingValue());
        c.missingValue(exp->missingValue());
        c.bitfieldDef(exp->bitfieldDef());
        columns_.push_back(c);
    }
}

} // namespace odb
