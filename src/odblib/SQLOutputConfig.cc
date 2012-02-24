#include "SQLOutputConfig.h"

namespace odb {
namespace sql {

const char* SQLOutputConfig::defaultDelimiter_("	");
const char* SQLOutputConfig::defaultOutputFile_("output.odb");
const char* SQLOutputConfig::defaultFormat_("default");

const SQLOutputConfig SQLOutputConfig::defaultConfig_;

} // namespace sql
} // namespace odb

