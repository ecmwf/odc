/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "machine.h"
#include "SQLBitfield.h"
#include "SQLODAOutput.h"
#include "DataStream.h"
#include "ODBAPISettings.h"
#include "SQLSession.h"
#include "TemplateParameters.h"
#include "Codec.h"
#include "Column.h"
#include "IteratorProxy.h"
#include "Writer.h"
#include "RowsIterator.h"
#include "WriterDispatchingIterator.h"
#include "DispatchingWriter.h"
#include "SQLOutput.h"
#include "SQLDistinctOutput.h"
#include "SQLOrderOutput.h"
#include "SQLDatabase.h"

template class ThreadSingleton<odb::ODBAPISettings>;
static ThreadSingleton<odb::ODBAPISettings> instance_;

namespace odb {

ODBAPISettings& ODBAPISettings::instance()
{
	ASSERT( &instance_.instance() != 0 );
	return instance_.instance();
}

ODBAPISettings::ODBAPISettings()
: headerBufferSize_(Resource<long>("$ODB_HEADER_BUFFER_SIZE;-headerBufferSize;headerBufferSize", MEGA(4))),
  setvbufferSize_(Resource<long>("$ODB_SETVBUFFER_SIZE;-setvbufferSize;setvbufferSize", MEGA(8)))
{}

size_t ODBAPISettings::headerBufferSize() { return headerBufferSize_; }
void ODBAPISettings::headerBufferSize(size_t n) { headerBufferSize_ = n; }

size_t ODBAPISettings::setvbufferSize() { return setvbufferSize_; }
void ODBAPISettings::setvbufferSize(size_t n) { setvbufferSize_ = n; }

} // namespace odb
