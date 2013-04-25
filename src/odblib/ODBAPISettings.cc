/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"
#include "eckit/filesystem/AIOHandle.h"

#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/DataStream.h"
#include "odblib/DispatchingWriter.h"
#include "odblib/IteratorProxy.h"
#include "odblib/ODBAPISettings.h"
#include "odblib/RowsIterator.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLDatabase.h"
#include "odblib/SQLDistinctOutput.h"
#include "odblib/SQLODAOutput.h"
#include "odblib/SQLOrderOutput.h"
#include "odblib/SQLOutput.h"
#include "odblib/SQLSession.h"
#include "odblib/TemplateParameters.h"
#include "odblib/Writer.h"
#include "odblib/WriterDispatchingIterator.h"

using namespace eckit;

template class ThreadSingleton<odb::ODBAPISettings>;
static ThreadSingleton<odb::ODBAPISettings> instance_;

bool odb::ODBAPISettings::debug = false;

void debugMeNow() {
	Log::info() << "Debug me now" << endl;
	odb::ODBAPISettings::debug = true;
}

namespace odb {

ODBAPISettings& ODBAPISettings::instance()
{
	ASSERT( &instance_.instance() != 0 );
	return instance_.instance();
}

ODBAPISettings::ODBAPISettings()
: headerBufferSize_(Resource<long>("$ODB_HEADER_BUFFER_SIZE;-headerBufferSize;headerBufferSize", MEGA(4))),
  setvbufferSize_(Resource<long>("$ODB_SETVBUFFER_SIZE;-setvbufferSize;setvbufferSize", MEGA(8))),
  useAIO_(Resource<bool>("$ODB_API_USE_AIO", false))
{}

size_t ODBAPISettings::headerBufferSize() { return headerBufferSize_; }
void ODBAPISettings::headerBufferSize(size_t n) { headerBufferSize_ = n; }

size_t ODBAPISettings::setvbufferSize() { return setvbufferSize_; }
void ODBAPISettings::setvbufferSize(size_t n) { setvbufferSize_ = n; }

DataHandle* ODBAPISettings::writeToFile(const PathName& fn, const Length& length)
{
	DataHandle* h = 0;
	h = useAIO_ ? static_cast<DataHandle*>(new AIOHandle(fn)) : static_cast<DataHandle*>(new FileHandle(fn));
	h->openForWrite(length);
	return h;
}

DataHandle* ODBAPISettings::appendToFile(const PathName& fn, const Length& length)
{
	DataHandle *h = 0;
	h = useAIO_ ? static_cast<DataHandle*>(new AIOHandle(fn)) : static_cast<DataHandle*>(new FileHandle(fn));
	h->openForAppend(length);
	return h;
}

} // namespace odb
