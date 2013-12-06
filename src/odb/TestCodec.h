/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestCodec.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestCodec_H
#define TestCodec_H

#include "odblib/Codec.h"

namespace odb {
namespace tool {
namespace test {

#include "odblib/TestCase.h"


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

