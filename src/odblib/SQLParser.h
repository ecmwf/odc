// File SQLParser.h
// Baudouin Raoult - ECMWF Mar 98

#ifndef SQLParser_H
#define SQLParser_H

#include "SQLOutputConfig.h"

class PathName;
class DataHandle;

namespace odb {
namespace sql {

class SQLParser {
public:

	static int line();
	static void include(const PathName&);

	static void parseFile(const PathName&, DataHandle*);
	static void parseFile(const PathName&, DataHandle*, SQLOutputConfig);
	static void parseString(const string&, DataHandle*, SQLOutputConfig);

	static void parseFile(const PathName&, istream*);
	static void parseFile(const PathName&, istream*, SQLOutputConfig);
	static void parseString(const string&, istream*, SQLOutputConfig);


	static void parseFile(const PathName&, SQLDatabase&, SQLOutputConfig);
	static void parseString(const string&, SQLDatabase&, SQLOutputConfig);
};

} // namespace sql
} // namespace odb

#endif
