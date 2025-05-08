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
/// @author Piotr Kuchta
/// @author Simon Smart
/// @date Oct 2010

#ifndef odc_TextReader_H
#define odc_TextReader_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

#include "eckit/memory/NonCopyable.h"

#include "odc/IteratorProxy.h"

namespace eckit {
class PathName;
}

namespace odc {

//----------------------------------------------------------------------------------------------------------------------

class TextReaderIterator;

class TextReader : private eckit::NonCopyable {
public:

    typedef IteratorProxy<TextReaderIterator, TextReader, double> iterator;
    typedef iterator::Row row;

    TextReader(std::istream&, const std::string& delimiter);
    TextReader(const std::string& path, const std::string& delimiter);

    TextReader(TextReader&&);
    TextReader& operator=(TextReader&&);

    virtual ~TextReader();

    iterator begin();
    iterator end() const;

    std::istream& stream() { return *in_; }

    // For C API
    //	TextReaderIterator* createReadIterator(const eckit::PathName&);

#ifdef SWIGPYTHON
    iterator __iter__() { return begin(); }
#endif

    const std::string& delimiter() { return delimiter_; }

private:

    std::istream* in_;
    bool deleteDataHandle_;
    std::string delimiter_;

    // This is a bit nasty. The TextReader currently assumes that the data will only be
    // iterated _once_ (hence initiated by an istream). Therefore only create the iterator
    // once so that the MetaData doesn't get read from the stream multiple times.
    iterator iteratorSingleton_;

    friend class odc::IteratorProxy<odc::TextReaderIterator, odc::TextReader, double>;
    friend class odc::TextReaderIterator;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace odc

#endif
