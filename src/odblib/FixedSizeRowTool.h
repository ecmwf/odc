#ifndef FixedSizeRowTool_H
#define FixedSizeRowTool_H

namespace odb {
namespace tool {

class FixedSizeRowTool : public Tool {
public:
	FixedSizeRowTool (int argc, char *argv[]); 

	void run(); 
	
	static void help(ostream &o)
	{ o << "Converts file to a format with fixed size rows"; }

	static void usage(const string& name, ostream &o)
	{ o << name << " <input.odb> <output.odb>"; }

private:
// No copy allowed

    FixedSizeRowTool(const FixedSizeRowTool&);
    FixedSizeRowTool& operator=(const FixedSizeRowTool&);
};

template <> struct ExperimentalTool<FixedSizeRowTool> { enum { experimental = true }; };

} // namespace tool 
} // namespace odb 

#endif 
