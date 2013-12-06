/// @file   DataSelectSession.h
/// @author Tomas Kral

#ifndef DATASELECTSESSION_H_
#define DATASELECTSESSION_H_

#include "odblib/SQLSession.h"

namespace odb {
namespace internal {

class DataSelectIterator;

class DataSelectSession : public odb::sql::SQLSession
{
public:
    DataSelectSession(DataSelectIterator& it);
   ~DataSelectSession();

    odb::sql::SQLStatement* statement();

private:
    DataSelectSession(const DataSelectSession&);
    DataSelectSession& operator=(const DataSelectSession&);

    void statement(odb::sql::SQLStatement* sql);
    odb::sql::SQLOutput* defaultOutput();

    DataSelectIterator& it_;
    odb::sql::SQLStatement* sql_;
};

} // namespace internal
} // namespace odb

#endif // DATASELECTSESSION_H_
