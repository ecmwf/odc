/// \file TestCodec.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestCodec_H
#define TestCodec_H

#include "Codec.h"

namespace odb {
namespace tool {
namespace test {

class TestCodec : public TestCase {
public:
	TestCodec(int argc, char **argv);
	virtual ~TestCodec();
	
	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
	odb::codec::Codec& codec() { return *codec_; };
	odb::codec::Codec& codec(odb::codec::Codec* c) { delete codec_; codec_ = c; return *codec_; };

	odb::codec::Codec *codec_;
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

