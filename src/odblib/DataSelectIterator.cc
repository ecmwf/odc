/// @file   DataSelectIterator.h
/// @author Tomas Kral

#include "odblib/DataSelectIterator.h"

//#include "odblib/SQLDatabase.h"
//#include "odblib/SQLExpression.h"
#include "odblib/SQLParser.h"
#include "odblib/SQLSelect.h"

//#include "odblib/SQLDataSet.h"
#include "odblib/DataSelect.h"
//#include "odblib/DataColumns.h"
#include "odblib/DataSelectSession.h"

using namespace odb;

namespace odb {
namespace internal {

DataSelectIterator::DataSelectIterator(const DataSelect& query, bool begin)
  : query_(query),
    session_(begin ? createSession() : 0),
    select_(begin ? dynamic_cast<sql::SQLSelect*>(session_->statement()) : 0),
    row_(begin ? select_->results().size() : 0),
    aggregateResult_(false),
    noMore_(begin ? false : true)
{
    if (begin)
    {
        ASSERT(session_);
        ASSERT(select_);
    }
}

void DataSelectIterator::prepare()
{
    ASSERT(select_);
    select_->pushFirstFrame();
    increment();
}

const Expressions& DataSelectIterator::results() const
{
    ASSERT(select_);
    return select_->results();
}

DataSelectIterator::~DataSelectIterator()
{
    delete session_;
    delete select_;
}

DataSelectSession* DataSelectIterator::createSession()
{
    DataSelectSession* session = new DataSelectSession(*this);
    sql::SQLDatabase* db = query_.database();

    ASSERT(db);
    ASSERT(session);

    sql::SQLParser parser;
    parser.parseString(query_.statement(), *db, sql::SQLOutputConfig::defaultConfig());

    sql::SQLStatement* statement = session->statement();
    sql::SQLSelect* select = dynamic_cast<sql::SQLSelect*>(statement);
    select->prepareExecute();

    return session;
}

bool DataSelectIterator::equal(const DataSelectIterator&) const
{
    return noMore_;
}

void DataSelectIterator::increment()
{
    if (noMore_ || aggregateResult_)
    {
        noMore_ = true;
        return;
    }

    bool ok = select_->processOneRow();

    if (!ok)
    {
        if (select_->isAggregate())
        {
            aggregateResult_ = true;
        }
        else
            noMore_ = true;

        select_->postExecute();
    }
}

} // namespace internal
} // namespace odb
