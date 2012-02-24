#include "Log.h"
#include "SQLAST.h"
#include "SQLBitfield.h"
#include "SchemaAnalyzer.h"
#include "SQLUser.h"
#include "SQLServerSession.h"

namespace odb {
namespace sql {

SQLUser::SQLUser(TCPSocket& protocol):
	NetUser(protocol)
{}

SQLUser::~SQLUser() {}

void SQLUser::serve(Stream&, istream& in,ostream& out)
{
	SQLServerSession session(in,out);
	session.serve();
}

} // namespace sql
} // namespace odb
