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
/// \file DispatchingWriter.cc
///
/// @author Piotr Kuchta, June 2009

#include "eckit/config/Resource.h"
#include "odb_api/odblib/DispatchingWriter.h"

using namespace eckit;

namespace odb {

DispatchingWriter::DispatchingWriter(const std::string& outputFileTemplate, int maxOpenFiles, bool append)
: outputFileTemplate_(outputFileTemplate),
  maxOpenFiles_(maxOpenFiles ? maxOpenFiles : Resource<long>("$ODBAPI_MAX_OPEN_FILES;-maxOpenFiles;maxOpenFiles", 250)),
  append_(append)
{}

DispatchingWriter::~DispatchingWriter() {}

DispatchingWriter::iterator_class* DispatchingWriter::writer()
{
        NOTIMP;
        // TODO:
        return 0;
}

DispatchingWriter::iterator DispatchingWriter::begin()
{
	return iterator(new iterator_class(*this, maxOpenFiles_, append_));
}

} // namespace odb

