#ifndef SetTool_H
#define SetTool_H

namespace odb {
namespace tool {

class SetTool : public Tool {
public:
	SetTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o)
	{ o << "Creates a new file setting columns to given values"; }

	static void usage(const string& name, ostream &o)
	{ o << name << " <update-list> <input.odb> <output.odb>"; }

private:
// No copy allowed

    SetTool(const SetTool&);
    SetTool& operator=(const SetTool&);

	void parseUpdateList(string s, vector<std::string>& columns, vector<double>& values);
};

} // namespace tool 
} // namespace odb 

#endif 

