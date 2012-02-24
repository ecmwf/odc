#ifndef ImportODBTool_H
#define ImportODBTool_H

#define SRC __FILE__, __LINE__

class ODBIterator;
class Application;
class CommandLineParser;

#include "OldODBReader.h"

namespace odb {
namespace tool {

template <typename IN = odb::tool::OldODBReader>
class ImportODBTool : public Tool {
public:
	ImportODBTool (int argc, char *argv[]); 
	ImportODBTool (const Application *); 
	ImportODBTool (const CommandLineParser&); 

	void run(); 

protected:

	template <typename OUT_ITERATOR>
	unsigned long long saveData(OUT_ITERATOR w, PathName odb, std::string sql);
	
	typedef pair<unsigned long long, const vector<PathName> > DispatchResult;

	DispatchResult importDispatching(PathName db, const std::string& sql, const std::string& dumpFile);

	void validate(PathName db, const string& sql, const PathName& file);
	void validateRowsNumber(unsigned long long, const vector<PathName>&);

	void archiveFiles(const vector<PathName>&);
private:

// No copy allowed
    ImportODBTool(const ImportODBTool&);
    ImportODBTool& operator=(const ImportODBTool&);

	bool isECFSPathName(const PathName fileName);
	PathName readFromECFS(const PathName fileName);
};

} // namespace tool 
} //namespace odb 

#include "ImportODBTool.cc"

#endif 
