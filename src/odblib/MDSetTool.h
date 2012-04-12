#ifndef MDSetTool_H
#define MDSetTool_H

namespace odb {
namespace tool {

class MDSetTool : public Tool {
public:
	MDSetTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o)
	{ o << "Creates a new file resetting types of columns"; }

	static void usage(const string& name, ostream &o)
	{ o << name << " <update-list> <input.odb> <output.odb>"; }

private:
// No copy allowed

    MDSetTool(const MDSetTool&);
    MDSetTool& operator=(const MDSetTool&);

	void parseUpdateList(string s, vector<std::string>& columns, vector<std::string>& values);
};

} // namespace tool 
} // namespace odb 

#endif 

