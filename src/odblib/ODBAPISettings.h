#ifndef ODBAPISettings_H
#define ODBAPISettings_H

#include "SQLExpression.h"
#include "SQLSelect.h"
#include "SQLTable.h"
#include "DataHandle.h"
#include "ThreadSingleton.h"
#include "SQLOutputConfig.h"

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
