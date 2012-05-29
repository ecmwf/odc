#ifndef LSTool_H
#define LSTool_H

#include "odblib/Tool.h"

namespace odb {
namespace tool {

class LSTool : public Tool {
public:
	LSTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o)
	{ o << "Shows file's contents"; }

	static void usage(const string& name, ostream &o)
	{ o << name << " [-o <output-file>] <file-name>" << endl << endl; }

protected:
	virtual unsigned long long runFast(const string &db, ostream &out);

private:
// No copy allowed
    LSTool(const LSTool&);
    LSTool& operator=(const LSTool&);

	static const string nullString;
};

} // namespace tool 
} // namespace odb 

#endif 
