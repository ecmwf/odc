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

#include "ecml/data/DataHandleFactory.h"
#include "odb_api/Select.h"
#include "eckit/io/DataHandle.h"
#include "odb_api/SQLOutputConfig.h"

using namespace std;
using namespace eckit;
using namespace ecml;

namespace odb {

Select::Select(const std::string& selectStatement, DataHandle &dh)
: dataHandle_(&dh),
  deleteDataHandle_(false),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement),
  context_(0),
  ownSession_(ownSession(",")),
  outerSession_(ownSession_)
{}

Select::Select(const std::string& selectStatement, DataHandle &dh, ecml::ExecutionContext* context)
: dataHandle_(&dh),
  deleteDataHandle_(false),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement),
  context_(context),
  ownSession_(ownSession(",")), // TODO: get csv_delimiter from context
  outerSession_(ownSession_)
{}

Select::Select(const std::string& selectStatement, std::istream &is, const std::string& delimiter)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(&is),
  deleteIStream_(false),
  selectStatement_(selectStatement),
  delimiter_(delimiter),
  context_(0),
  ownSession_(ownSession(delimiter)),
  outerSession_(ownSession_)
{}

Select::Select(const std::string& selectStatement, std::istream &is, const std::string& delimiter, ecml::ExecutionContext* context)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(&is),
  deleteIStream_(false),
  selectStatement_(selectStatement),
  delimiter_(delimiter),
  context_(context),
  ownSession_(ownSession(delimiter)),
  outerSession_(ownSession_)
{}

Select::Select(const std::string& selectStatement)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement),
  context_(0),
  ownSession_(ownSession(",")),
  outerSession_(ownSession_)
{}

Select::Select(const std::string& selectStatement, odb::sql::SQLNonInteractiveSession& s)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement),
  context_(0),
  ownSession_(ownSession(s.csvDelimiter())),
  outerSession_(&s)
{}

Select::Select(const std::string& selectStatement, ecml::ExecutionContext* context)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement),
  context_(context),
  ownSession_(ownSession(",")),  // TODO: get csv_delimiter from context
  outerSession_(ownSession_)
{}

Select::Select()
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(),
  context_(0),
  ownSession_(ownSession(",")),
  outerSession_(ownSession_)
{}

Select::Select(ecml::ExecutionContext* context)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(),
  context_(context),
  ownSession_(ownSession(",")), // TODO: get csv_delimiter from context
  outerSession_(ownSession_)
{}

Select::Select(const std::string& selectStatement, const std::string& path)
: dataHandle_(DataHandleFactory::openForRead(path)),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement),
  context_(0),
  ownSession_(ownSession(",")),
  outerSession_(ownSession_)
{
    //dataHandle_->openForRead();
}

Select::Select(const std::string& selectStatement, const std::string& path, ecml::ExecutionContext* context)
: dataHandle_(DataHandleFactory::openForRead(path)),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement),
  context_(context),
  ownSession_(ownSession(",")),  // TODO: get csv_delimiter from context
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

SelectIterator* Select::createSelectIterator(const std::string& sql, ecml::ExecutionContext* context)
{
    return new SelectIterator(*this, sql, context, *outerSession_);
}

const Select::iterator Select::end() { return iterator(0); }

Select::iterator Select::begin()
{
    SelectIterator* it = new SelectIterator(*this, selectStatement_, context_, *outerSession_);
    ASSERT(it);
    it->next(context_);
    return iterator(it);
}

odb::sql::SQLNonInteractiveSession* Select::ownSession(const std::string& delimiter)
{ 
    return new odb::sql::SQLNonInteractiveSession(odb::sql::SQLOutputConfig::defaultConfig(), delimiter); 
}

#ifdef SWIGPYTHON
template odb::IteratorProxy< odb::SelectIterator,odb::Select,double const >; 
#endif

} // namespace odb
