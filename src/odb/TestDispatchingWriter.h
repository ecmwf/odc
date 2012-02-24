/// \file TestDispatchingWriter.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#ifndef TestDispatchingWriter_H
#define TestDispatchingWriter_H

namespace odb {
namespace tool {
namespace test {

class TestDispatchingWriter : public TestCase {
public:
	TestDispatchingWriter(int argc, char **argv);
	~TestDispatchingWriter();

	virtual void setUp();
	virtual void test();
	virtual void tearDown();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

