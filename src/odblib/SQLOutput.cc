#include "SQLOutput.h"
#include "SQLOutputConfig.h"

namespace odb {
namespace sql {

SQLOutput::SQLOutput() {}

SQLOutput::~SQLOutput() {}

void SQLOutput::print(ostream& s) const
{
	s << "SQLOutput" << endl;
}

const SQLOutputConfig& SQLOutput::config() { return config_; }

void SQLOutput::config(SQLOutputConfig& cfg) { config_ = cfg; }

} // namespace sql
} // namespace odb
