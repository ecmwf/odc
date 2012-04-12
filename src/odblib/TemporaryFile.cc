#include <sstream>

#include "eclib/Buffer.h"

#include "odblib/TemporaryFile.h"

TemporaryFile::TemporaryFile()
: PathName()
{
	stringstream t;
	t << (getenv("TMPDIR") ? getenv("TMPDIR") : "");
	if (t.str() != "")
		t << "/";

	const char *e = getenv("ODB_API_TMP");
	t << (e ? e : "ODB_API_TMP")
	  << ::getpid()
	// The last six characters of template must be "XXXXXX"
	  << "XXXXXX";

	Log::info() << "templatePath: " << t.str() << endl;
	PathName templatePath(t.str());
	Buffer tmpPath(templatePath);

	int fd = ::mkstemp(static_cast<char *>(tmpPath));
	if (fd == -1)
		throw SeriousBug("Could not create a unique temporary filename.");

	*static_cast<PathName*>(this) = static_cast<char *>(tmpPath);
}

TemporaryFile::~TemporaryFile()
{
	unlink();
}
