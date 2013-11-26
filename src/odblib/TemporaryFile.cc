/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sys/types.h>
#include <unistd.h>
#include <sstream>

#include "eckit/io/Buffer.h"

#include "odblib/TemporaryFile.h"

using namespace eckit;

TemporaryFile::TemporaryFile()
: PathName()
{
    std::stringstream t;
	t << (getenv("TMPDIR") ? getenv("TMPDIR") : "");
	if (t.str() != "")
		t << "/";

	const char *e = getenv("ODB_API_TMP");
	t << (e ? e : "ODB_API_TMP")
	  << ::getpid()
	// The last six characters of template must be "XXXXXX"
	  << "XXXXXX";

	Log::info() << "templatePath: " << t.str() << std::endl;
	PathName templatePath(t.str());
	Buffer tmpPath(templatePath);

	int fd = ::mkstemp(static_cast<char *>(tmpPath));
	if (fd == -1)
		throw eckit::SeriousBug("Could not create a unique temporary filename.");

	*static_cast<PathName*>(this) = static_cast<char *>(tmpPath);
}

TemporaryFile::~TemporaryFile()
{
	unlink();
}
