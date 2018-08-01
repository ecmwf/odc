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
/// \file Select.cc
///
/// @author Piotr Kuchta, April 2010

#include "odb_api/data/DataHandleFactory.h"
#include "odb_api/Select.h"
#include "eckit/io/DataHandle.h"
#include "eckit/sql/SQLOutputConfig.h"

using namespace std;
using namespace eckit;

namespace odb {

Select::Select(const std::string& selectStatement, DataHandle &dh)
: dataHandle_(&dh),
  deleteDataHandle_(false),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement),
  ownSession_(ownSession(",")),
  outerSession_(ownSession_)
{}

Select::Select(const std::string& selectStatement, std::istream &is, const std::string& delimiter)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(&is),
  deleteIStream_(false),
  selectStatement_(selectStatement),
  delimiter_(delimiter),
  ownSession_(ownSession(delimiter)),
  outerSession_(ownSession_)
{}

Select::Select(const std::string& selectStatement)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement),
  ownSession_(ownSession(",")),
  outerSession_(ownSession_)
{}

Select::Select(const std::string& selectStatement, eckit::sql::SQLSession& s)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement),
  ownSession_(ownSession(s.csvDelimiter())),
  outerSession_(&s)
{}

Select::Select()
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(),
  ownSession_(ownSession(",")),
  outerSession_(ownSession_)
{}

Select::Select(const std::string& selectStatement, const std::string& path)
: dataHandle_(DataHandleFactory::openForRead(path)),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement),
  ownSession_(ownSession(",")),
  outerSession_(ownSession_)
{
    //dataHandle_->openForRead();
}

Select::~Select()
{
    if (deleteDataHandle_) delete dataHandle_;
    if (deleteIStream_) delete istream_;
    delete ownSession_;
}

SelectIterator* Select::createSelectIterator(const std::string& sql)
{
    return new SelectIterator(*this, sql, *outerSession_);
}

const Select::iterator Select::end() { return iterator(0); }

Select::iterator Select::begin()
{
    SelectIterator* it = new SelectIterator(*this, selectStatement_, *outerSession_);
    ASSERT(it);
    it->next();
    return iterator(it);
}

eckit::sql::SQLSession* Select::ownSession(const std::string& delimiter)
{ 
    return new eckit::sql::SQLSession(eckit::sql::SQLOutputConfig::defaultConfig(), delimiter);
}

#ifdef SWIGPYTHON
template odb::IteratorProxy< odb::SelectIterator,odb::Select,double const >; 
#endif

} // namespace odb
