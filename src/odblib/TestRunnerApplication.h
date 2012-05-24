/// \file TestRunnerApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestRunnerApplication_H
#define TestRunnerApplication_H

#include "odblib/ODBApplication.h"

namespace odb {
namespace tool {
namespace test {

class TestRunnerApplication : public ODBApplication {
public:
	TestRunnerApplication (int argc, char **argv);
	virtual ~TestRunnerApplication ();

	void run();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif
