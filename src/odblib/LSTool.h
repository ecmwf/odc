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
	{
		o << name
			<< " [-o <text-output-file>]"
			<< " [-O <ODA-output-file>]"
			<< " [-s <list-of-columns-to-be-selected>]"
			<< " [-w <filtering-condition>]"
			<< " <file-name>"
			<< endl << endl
			<< "The default list of selected columns is: '" 
			<< defaultColumns
			<< "'"
			;
	}

protected:
	
	virtual unsigned long long runSQLSelect(const string &db, const string &selectList, const string &whereClause, ostream &out, const string & = nullString);
	virtual unsigned long long runFast(const string &db, const vector<string> &columns, ostream &out);

	static const char * defaultColumns;
private:

// No copy allowed

    LSTool(const LSTool&);
    LSTool& operator=(const LSTool&);

	static const string nullString;
};

} // namespace tool 
} // namespace odb 

#endif 
