#ifndef CompactTool_H
#define CompactTool_H

namespace odb {
namespace tool {

class CompactTool : public Tool {
public:
	CompactTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o)
	{ o << "Tries to compress a file"; }

	static void usage(const string& name, ostream &o)
	{ o << name << " <input.odb> <output.odb>"; }

private:
// No copy allowed

    CompactTool(const CompactTool&);
    CompactTool& operator=(const CompactTool&);
};

template <> struct ExperimentalTool<CompactTool> { enum { experimental = true }; };

} // namespace tool 
} // namespace odb 

#endif 
