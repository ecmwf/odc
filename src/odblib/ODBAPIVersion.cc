///
/// This file will be updated autmatically by the make.sms script
///

#include "oda.h"

namespace odb {

	const char *ODBAPIVersion::version() { return "0.9.11"; }

	unsigned int ODBAPIVersion::formatVersionMajor() { return FORMAT_VERSION_NUMBER_MAJOR; }
	unsigned int ODBAPIVersion::formatVersionMinor() { return FORMAT_VERSION_NUMBER_MINOR ;}

} // namespace odb

