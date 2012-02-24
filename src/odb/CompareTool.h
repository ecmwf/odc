#ifndef CompareTool_H
#define CompareTool_H

namespace odb {
namespace tool {

class CompareTool : public Tool {
public:
	CompareTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o)
	{
		o << "Compares two files";
	}

	static void usage(const string& name, ostream &o)
	{
		o << name << " <file1.oda> <file2.oda>";
	}

private:
// No copy allowed

    CompareTool(const CompareTool&);
    CompareTool& operator=(const CompareTool&);

	static char* dummyArgv_[];

	odb::Comparator comparator;

	PathName* file1;
	PathName* file2;

	odb::RowsReaderIterator* reader1_;
	odb::RowsReaderIterator* reader2_;

};

} // namespace tool 
} // namespace odb 

#endif 
