#ifndef HeaderTool_H
#define HeaderTool_H

namespace odb {
namespace tool {

class HeaderTool : public Tool {
public:
	HeaderTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o)
	{
		o << "Shows header(s) and metadata(s) of file";
	}

	static void usage(const string& name, ostream &o)
	{
		o << name << " <file-name>";
	}

private:

// No copy allowed

    HeaderTool(const HeaderTool&);
    HeaderTool& operator=(const HeaderTool&);

	
	string readFile(const string &fileName);
};

} // namespace tool 
} // namespace odb 

#endif 
