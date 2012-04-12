#ifndef MergeTool_H
#define MergeTool_H

namespace odb {
namespace tool {

class MergeTool : public Tool {
public:
	MergeTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o) { o << "Merges rows from two files"; }
	static void usage(const string& name, ostream &o)
	{
		o << name << " <file1.odb> <file2.odb> <output-file.odb>";
	}

	static void merge(const PathName& file1, const PathName& file2, const PathName& outputFileName);

private:
// No copy allowed

    MergeTool(const MergeTool&);
    MergeTool& operator=(const MergeTool&);

	static char* dummyArgv_[];

	PathName file1_;
	PathName file2_;
	PathName outputFile_;

};

} // namespace tool 
} // namespace odb 

#endif 
