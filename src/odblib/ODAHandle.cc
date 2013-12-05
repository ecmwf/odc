/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/ODAHandle.h"
#include "odblib/ODATranslator.h"

using namespace eckit;

ODAHandle::ODAHandle(Offset start, Offset end)
: start_(start),
  end_(end)
{
	Log::debug() << "ODAHandle::ODAHandle(" << start << ", " << end << ")" << std::endl; 
}

void ODAHandle::print(std::ostream& o) const
{
	o << "[start:" << start_<< ", end_:" << end_ << ", values_:" << values_ << "]";
}

ODAHandle::~ODAHandle()
{
	Log::debug() << "ODAHandle::~ODAHandle()" << std::endl;
}

void ODAHandle::addValue(const std::string& columnName, double v)
{
	Log::debug() << "ODAHandle::addValue('" << columnName << "', '" << v << "')" << std::endl;
	ASSERT(values_.find(columnName) == values_.end());
	values_[columnName] = v;
}

