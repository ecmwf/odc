/// @file   DataSelectSession.cc
/// @author Tomas Kral

#include "eckit/exception/Exceptions.h"
#include "odb_api/DataSelectOutput.h"
#include "odb_api/DataSelectSession.h"
#include "eckit/sql/SQLOutputConfig.h"

namespace odc {
namespace internal {

DataSelectSession::DataSelectSession(DataSelectIterator& it)
 : SQLSession(odc::sql::SQLOutputConfig::defaultConfig(), ","),
   it_(it), 
   sql_(0)
{
    loadDefaultSchema();
}

DataSelectSession::~DataSelectSession()
{}

void DataSelectSession::statement(odc::sql::SQLStatement* sql)
{
    ASSERT(sql);    
    sql_ = sql;
    gotSelectAST(false);
}

odc::sql::SQLOutput* DataSelectSession::defaultOutput()
{
    return new DataSelectOutput(it_);
}

odc::sql::SQLStatement* DataSelectSession::statement()
{
    typedef odc::sql::SQLStatement* P;
    if (gotSelectAST())
    {
        gotSelectAST(false);
        sql_ = P(selectFactory().create(*this, selectAST()));
    }
    return sql_;
}

} // namespace internal
} // namespace odc
