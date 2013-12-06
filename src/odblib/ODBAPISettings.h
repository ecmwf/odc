/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ODBAPISettings_H
#define ODBAPISettings_H

//#include "eckit/io/DataHandle.h"
//#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/ThreadSingleton.h"

//#include "odblib/SQLExpression.h"
//#include "odblib/SQLOutputConfig.h"
//#include "odblib/SQLSelect.h"
//#include "odblib/SQLTable.h"

namespace odb {

class ODBAPISettings : private eckit::NonCopyable {
public:
    
	static ODBAPISettings& instance();

	size_t headerBufferSize();
	void headerBufferSize(size_t);

	size_t setvbufferSize();
	void setvbufferSize(size_t);

    eckit::DataHandle* writeToFile(const eckit::PathName&, const eckit::Length& = eckit::Length(0), bool openDataHandle = true);
    eckit::DataHandle* appendToFile(const eckit::PathName&, const eckit::Length& = eckit::Length(0), bool openDataHandle = true);

	static bool debug;

private:

    ODBAPISettings();

	size_t headerBufferSize_;
	size_t setvbufferSize_;

	bool useAIO_;

    friend class eckit::NewAlloc0<ODBAPISettings>;
};

} // namespace odb

#endif
