/// @file   DataSelectSession.h
/// @author Tomas Kral

#ifndef DATASELECTSESSION_H_
#define DATASELECTSESSION_H_

#include "eckit/sql/SQLSession.h"

namespace odc {
namespace internal {

class DataSelectIterator;

class DataSelectSession : public odc::sql::SQLSession
{
public:
    DataSelectSession(DataSelectIterator& it);
   ~DataSelectSession();

    odc::sql::SQLStatement* statement();

private:
    DataSelectSession(const DataSelectSession&);
    DataSelectSession& operator=(const DataSelectSession&);

    void statement(odc::sql::SQLStatement* sql);
    odc::sql::SQLOutput* defaultOutput();

    DataSelectIterator& it_;
    odc::sql::SQLStatement* sql_;
};

} // namespace internal
} // namespace odc

#endif // DATASELECTSESSION_H_
