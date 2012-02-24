/// \file TestMetaDataReader.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#ifndef TestMetaDataReader_H
#define TestMetaDataReader_H

namespace odb {
namespace tool {
namespace test {

class TestMetaDataReader : public TestCase {
public:
	TestMetaDataReader(int argc, char **argv);
	virtual ~TestMetaDataReader();
	
	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

