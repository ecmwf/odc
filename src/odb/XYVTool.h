#ifndef XYVTool_H
#define XYVTool_H

namespace odb {
namespace tool {

class XYVTool : public Tool {

public:
	XYVTool(int argc, char **argv);
	~XYVTool();

	static void help(ostream &o)
	{ o << "Creates XYV representation of file for displaying in a grphics program"; }

	static void usage(const string& name, ostream &o)
	{ o << name << " <input-file.odb> <value-column> <output-file.odb>"; }

	virtual void run();
};

template <> struct ExperimentalTool<XYVTool> { enum { experimental = true }; };

} // namespace tool 
} // namespace odb 

#endif

