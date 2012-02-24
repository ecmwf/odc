/// \file TestCommandLineParsing.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestCommandLineParsing_H
#define TestCommandLineParsing_H

namespace odb {
namespace tool {
namespace test {

class TestCommandLineParsing : public TestCase {
public:
	virtual void test();
	TestCommandLineParsing(int argc, char **argv);
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

