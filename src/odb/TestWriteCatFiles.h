/// \file TestWriteCatFiles.h
///
/// @author Piotr Kuchta, ECMWF, Jun 2011

#ifndef TestWriteCatFiles_H
#define TestWriteCatFiles_H

namespace odb {
namespace tool {
namespace test {

class TestWriteCatFiles : public TestCase {
public:
	TestWriteCatFiles(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestWriteCatFiles();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

