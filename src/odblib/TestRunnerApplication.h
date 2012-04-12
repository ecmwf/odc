/// \file TestRunnerApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestRunnerApplication_H
#define TestRunnerApplication_H

namespace odb {
namespace tool {
namespace test {

class TestRunnerApplication : public Application {
public:
	TestRunnerApplication (int argc, char **argv);
	virtual ~TestRunnerApplication ();

	void run();

private:
	int argc_;
	char** argv_;
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif
