/// \file TestSelectIterator.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestSelectIterator_H
#define TestSelectIterator_H

namespace odb {
namespace tool {
namespace test {

class TestSelectIterator : public TestCase {
public:
	TestSelectIterator(int argc, char **argv);

	/// Writes 10 rows to file test.oda.
	virtual void setUp();

	void testBug01();

	/// Reads 10 rows using ReaderIterator and for_each.
	void testReaderIteratorForEach();

	/// Reads 10 rows using ReaderIterator in a loop.
	void testReaderIteratorLoop();

	/// Reads 10 rows using SelectIterator and for_each
	void testSelectIteratorForEach();

	/// Reads 10 rows using SelectIterator in a loop.
	void testSelectIteratorLoop();


	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestSelectIterator();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

