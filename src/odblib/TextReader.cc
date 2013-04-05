/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file ODA.cc
///
/// @author Piotr Kuchta, Oct 2010

#include "odblib/TextReader.h"

#include "eclib/DataHandle.h"
#include "eclib/PathName.h"

#include "odblib/TextReaderIterator.h"

using namespace std;
using namespace eclib;

namespace odb {
#define MEGA(x) (x*1024*1024)

TextReader::TextReader(std::istream& input, const string& delimiter)
: in_(&input),
  deleteDataHandle_(false),
  delimiter_(delimiter)
{}

//TextReader::TextReader()
//: in_(0),
//  deleteDataHandle_(true),
//  path_("")
//{}

TextReader::TextReader(const string& path, const string& delimiter)
: in_(new std::ifstream(path.c_str())),
  deleteDataHandle_(true),
  path_(path),
  delimiter_(delimiter)
{
	//dataHandle_->openForRead();
}

TextReader::~TextReader()
{
	///if (dataHandle_ && deleteDataHandle_)
	if (in_ && deleteDataHandle_)
	{
		//dataHandle_->close();
		//delete dataHandle_;
		delete in_;
	}
}

TextReaderIterator* TextReader::createReadIterator(const PathName& pathName)
{
	return new TextReaderIterator(*this, pathName);
}

TextReader::iterator TextReader::begin()
{
	TextReaderIterator * it = new TextReaderIterator(*this);
	it->next();
	return iterator(it);
}

const TextReader::iterator TextReader::end() { return iterator(0); }

} // namespace odb
