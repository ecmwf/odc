#ifndef ImportTool_H
#define ImportTool_H

namespace odb {
namespace tool {

class ImportTool : public Tool {
public:
	ImportTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o)
	{ o << "Imports data from a text file"; }

	static void usage(const string& name, ostream &o)
	{ o << name << " [-d delimiter] <input.file> <output.file>"; }

	static void importFile(const PathName& in, const PathName& out, const string& delimiter = defaultDelimiter);
	static void importText(const string& s, const PathName& out, const string& delimiter = defaultDelimiter);
private:
// No copy allowed
    ImportTool(const ImportTool&);
    ImportTool& operator=(const ImportTool&);

	static string defaultDelimiter;
};

} // namespace tool 
} // namespace odb 

#endif 

