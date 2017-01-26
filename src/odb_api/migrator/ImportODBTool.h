/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odb_api_ImportODBTool_H
#define odb_api_ImportODBTool_H

#include <eckit/eckit.h>

#include "eckit/filesystem/PathName.h"
#include "odb_api/migrator/OldODBReader.h"
#include "odb_api/tools/Tool.h"


class ODBIterator;
class CommandLineParser;



namespace odb {
namespace tool {

template <typename IN = odb::tool::OldODBReader>
class ImportODBTool : public Tool {
public:
	ImportODBTool (int argc, char *argv[]); 
	ImportODBTool (const CommandLineParser&); 

	void run(); 

protected:

	template <typename OUT_ITERATOR>
	unsigned long long saveData(OUT_ITERATOR w, eckit::PathName odb, std::string sql);
	
    typedef std::pair<unsigned long long, const std::vector<eckit::PathName> > DispatchResult;

	DispatchResult importDispatching(eckit::PathName db, const std::string& sql, const std::string& dumpFile);

    void validate(eckit::PathName db, const std::string& sql, const eckit::PathName& file);
    void validateRowsNumber(unsigned long long, const std::vector<eckit::PathName>&);

    void archiveFiles(const std::vector<eckit::PathName>&);
private:

// No copy allowed
    ImportODBTool(const ImportODBTool&);
    ImportODBTool& operator=(const ImportODBTool&);

	bool isECFSPathName(const eckit::PathName fileName);
	eckit::PathName readFromECFS(const eckit::PathName fileName);
};

} // namespace tool 
} //namespace odb 

#include "ImportODBTool.cc"

#endif 
