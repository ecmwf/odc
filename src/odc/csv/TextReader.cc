/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "odc/csv/TextReader.h"
#include "odc/csv/TextReaderIterator.h"


using namespace eckit;

namespace odc {

//----------------------------------------------------------------------------------------------------------------------

TextReader::TextReader(std::istream& input, const std::string& delimiter) :
    in_(&input),
    deleteDataHandle_(false),
    delimiter_(delimiter),
    iteratorSingleton_(new TextReaderIterator(*this)) {}


TextReader::TextReader(const std::string& path, const std::string& delimiter) :
    in_(new std::ifstream(path.c_str())),
    deleteDataHandle_(true),
    delimiter_(delimiter),
    iteratorSingleton_(new TextReaderIterator(*this)) {}

TextReader::TextReader(TextReader&& rhs) :
    in_(rhs.in_),
    deleteDataHandle_(rhs.deleteDataHandle_),
    delimiter_(rhs.delimiter_),
    iteratorSingleton_(rhs.iteratorSingleton_) {

    rhs.in_ = 0;
    rhs.deleteDataHandle_ = false;
}

TextReader& TextReader::operator=(TextReader&& rhs) {
    std::swap(in_, rhs.in_);
    std::swap(deleteDataHandle_, rhs.deleteDataHandle_);
    std::swap(delimiter_, rhs.delimiter_);
    std::swap(iteratorSingleton_, rhs.iteratorSingleton_);
    return *this;
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

//TextReaderIterator* TextReader::createReadIterator(const PathName& pathName)
//{
//	return new TextReaderIterator(*this, pathName);
//}

TextReader::iterator TextReader::begin()
{
    /// @note YUCK. THis is not a good idom. begin() and end() don't really work...
    return iteratorSingleton_;
}

TextReader::iterator TextReader::end() const { return iterator(0); }

//----------------------------------------------------------------------------------------------------------------------

} // namespace odc
