/// \file Tool.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef Tool_H
#define Tool_H

#include <map>
#include <set>
#include <vector>

#include "Tokenizer.h"
#include "StringTool.h"
#include "CommandLineParser.h"

using namespace std;

class PathName;
class Application;

namespace odb {
namespace tool {

class Tool : public StringTool, public CommandLineParser {
public:
	virtual void run() = 0;

	virtual ~Tool();

	string name() { return name_; }
	void name(string s) { name_ = s; }

protected:
	Tool(int argc, char **argv);
	Tool(const Application *);
	Tool(const CommandLineParser&);

private:
	string name_;
};


template <typename T> struct ExperimentalTool { enum { experimental = false }; };

} // namespace tool 
} // namespace odb 

#endif

