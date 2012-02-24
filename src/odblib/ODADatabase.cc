#include <cctype>
#include "FILEHandle.h"

#include "oda.h"
#include "ODADatabase.h"
#include "TODATable.h"
#include "StringTool.h"

namespace odb {
namespace sql {

ODADatabase::ODADatabase(const PathName& path,const string& name)
: SQLDatabase(path,name)
{}

ODADatabase::~ODADatabase() {}

void ODADatabase::open() {}

void ODADatabase::close() { SQLDatabase::close(); }

SQLTable* ODADatabase::table(const string& name)
{
	map<string,SQLTable*>::iterator j = tablesByName_.find(name);

	if(j == tablesByName_.end())
	{
		// FIXME (?): path_ is '.'. ignore now
		//tablesByName_[name] = new TODATable<Reader>(*this,path_,name);
		tablesByName_[name] = new TODATable<Reader>(*this,name,name);
		j = tablesByName_.find(name);
	}
	return (*j).second;
}

SQLTable* ODADatabase::openDataHandle(DataHandle& dh, DataFormat dataFormat)
{
	string name = "dataHandle@";
	//name += string(&dh);

	if (dataFormat == ODA) return tablesByName_[name] = new TODATable<Reader>(*this, dh);
	// TODO
	//if (dataFormat == CSV) return tablesByName_[name] = new TODATable<TextReader>(*this, dh);

	ASSERT(0 && "Format not supported");
	return 0;
}

SQLTable* ODADatabase::openDataStream(istream& is, DataFormat dataFormat) 
{
	string name = "dataHandle@";
	//name += string(&dh);

	if (dataFormat == CSV) return tablesByName_[name] = new TODATable<TextReader>(*this, is);
	// TODO
	//if (dataFormat == CSV) return tablesByName_[name] = new TODATable<TextReader>(*this, dh);

	ASSERT(0 && "Format not supported");
	return 0;
}

} // namespace sql
} // namespace odb

