#ifndef SplitTool_H
#define SplitTool_H

namespace odb {
namespace tool {

class SplitTool : public Tool {
public:
	SplitTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o)
	{
		o << "Splits file according to given template";
	}

	static void usage(const string& name, ostream &o)
	{
		o << name << " <input.odb> <output_template.odb>";
	}

private:
// No copy allowed

    SplitTool(const SplitTool&);
    SplitTool& operator=(const SplitTool&);
};

} // namespace tool 
} // namespace odb 

#endif 
