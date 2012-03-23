/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ODBAPISettings_H
#define ODBAPISettings_H

#include "eclib/DataHandle.h"
#include "eclib/ThreadSingleton.h"
#include "odblib/SQLExpression.h"
#include "odblib/SQLOutputConfig.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLTable.h"

namespace odb {

class ODBAPISettings {
public:

	static ODBAPISettings& instance();

	size_t headerBufferSize();
	void headerBufferSize(size_t);

	size_t setvbufferSize();
	void setvbufferSize(size_t);

private:
	ODBAPISettings();

// No copy allowed
	ODBAPISettings(const ODBAPISettings&);
	ODBAPISettings& operator=(const ODBAPISettings&);

	size_t headerBufferSize_;
	size_t setvbufferSize_;

friend class ThreadSingleton<ODBAPISettings>;
};

} // namespace odb

#endif
