#ifndef SQLServerTool_H
#define SQLServerTool_H

namespace odb {
namespace tool {

class SQLServerTool : public Tool {

public:
	SQLServerTool(int argc,char **argv);
	~SQLServerTool();

	static void help(ostream &o)
	{ o << "Starts a socket server serving SQL requests"; }

	static void usage(const string& name, ostream &o)
	{ o << name << " -port <port-number>"; }
	
	virtual void run();
};

template <> struct ExperimentalTool<SQLServerTool> { enum { experimental = true }; };

} // namespace tool 
} // namespace odb 

#endif

