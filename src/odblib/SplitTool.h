/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
		o << name << " [-nosort] [-maxopenfiles <N>] <input.odb> <output_template.odb>";
	}

	static void split(const PathName&, const string&, size_t);
	static void presortAndSplit(const PathName&, const string&);

	static vector<pair<Offset,Length> > getChunks(const PathName&, size_t maxExpandedSize = 100*1024*1024);
private:
// No copy allowed
    SplitTool(const SplitTool&);
    SplitTool& operator=(const SplitTool&);

	static string genOrderBySelect(const string&, const string&);

	long maxOpenFiles_;
	bool sort_;
};

} // namespace tool 
} // namespace odb 

#endif 
