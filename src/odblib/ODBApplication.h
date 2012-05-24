/// \file ODBApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef ODBApplication_H
#define ODBApplication_H

#include "odblib/CommandLineParser.h"

namespace odb {
namespace tool {

class ODBApplication {
public:
	ODBApplication (int argc, char **argv);
	virtual ~ODBApplication ();

	virtual void start();
	virtual void run() = 0;

	CommandLineParser& commandLineParser();

private:
	CommandLineParser clp_;
};

} // namespace tool 
} // namespace odb 

#endif
