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
/// \file CallbackExecutor.cc
///
/// @author Piotr Kuchta, April 2010

#include "eckit/io/FileHandle.h"
#include "odb_api/SQLRowCallback.h"
#include "odb_api/CallbackExecutor.h"
#include "odb_api/SQLCallbackSession.h"
#include "odb_api/SQLOutputConfig.h"
#include "odb_api/SQLParser.h"
#include "odb_api/SQLSelectFactory.h"

using namespace std;
using namespace eckit;
using namespace odb::sql;

namespace odb {

CallbackExecutor::CallbackExecutor(const std::string& selectStatement, DataHandle &dh)
: dataHandle_(&dh),
  deleteDataHandle_(false),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement)
{}

CallbackExecutor::CallbackExecutor(const std::string& selectStatement, std::istream &is, const std::string& delimiter)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(&is),
  deleteIStream_(false),
  selectStatement_(selectStatement),
  delimiter_(delimiter)
{}

CallbackExecutor::CallbackExecutor(const std::string& selectStatement)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement)
{}

CallbackExecutor::CallbackExecutor()
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_()
{}

CallbackExecutor::CallbackExecutor(const std::string& selectStatement, const std::string& path)
: dataHandle_(new FileHandle(path)),
  deleteDataHandle_(true),
  // TODO: depending on file extension?
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement)
{}

CallbackExecutor::~CallbackExecutor()
{
    if (deleteDataHandle_) delete dataHandle_;
    if (deleteIStream_) delete istream_;
}

void CallbackExecutor::execute(odb::sql::SQLRowCallback& callback,void * aux)
{
    /** TODO
    Log::info() << "CallbackExecutor::execute: " << endl;

    //input.openForRead();

    SQLCallbackSession session;

    SQLOutputConfig config(SQLSelectFactory::instance().config());
    //config.outputFormat("odb");
    //config.outputFile(into);
    SQLParser parser;
    //parser.parseString(sql, &input, config);
    parser.parseString(selectStatement_, config);

    SQLSelect& sqlSelect(dynamic_cast<SQLSelect&>(session.statement()));

    long long numberOfRows (sqlSelect.execute());
    Log::info() << "Processed " << numberOfRows << " row(s)." << endl;
    */
}

} // namespace odb
