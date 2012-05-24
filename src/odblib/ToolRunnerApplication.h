/// \file ToolRunnerApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef ToolRunnerApplication_H
#define ToolRunnerApplication_H

#include "odblib/ODBApplication.h"

#include "odblib/Tool.h"

namespace odb {
namespace tool {

class ToolRunnerApplication : public ODBApplication {
public:
	ToolRunnerApplication (int argc, char **argv, bool createCommandLineTool = true, bool deleteTool=true);
	ToolRunnerApplication (Tool &tool, int argc, char **argv);
	~ToolRunnerApplication ();

	void tool(Tool *);

	void run();
	int printHelp(ostream &out);
private:
	Tool* tool_;
	bool deleteTool_;
};

} // namespace tool 
} // namespace odb 

#endif
