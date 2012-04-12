#ifndef CountTool_H
#define CountTool_H

namespace odb {
namespace tool {

class CountTool : public Tool {
public:
	CountTool (int argc, char *argv[]); 

	static unsigned long long rowCount(const PathName &);
	static unsigned long long fastRowCount(const PathName &);

	void run(); 

	static void help(ostream &o)
	{
		o << "Counts number of rows";
	}

	static void usage(const string& name, ostream &o)
	{
		o << name << " <file.odb>";
	}

private:

// No copy allowed

    CountTool(const CountTool&);
    CountTool& operator=(const CountTool&);
};

} // namespace tool 
} // namespace odb 

#endif 
