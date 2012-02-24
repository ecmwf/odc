/// \file TestSetvbuffer.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestSetvbuffer_H
#define TestSetvbuffer_H

namespace odb {
namespace tool {
namespace test {

class TestSetvbuffer : public TestCase {
public:
	TestSetvbuffer(int argc, char **argv);

	virtual void setUp();


	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestSetvbuffer();
private:
	void createFile(size_t numberOfColumns, long long numberOfRows, size_t);
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

