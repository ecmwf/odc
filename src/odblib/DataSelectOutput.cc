/// @file   DataSelectOutput.cc
/// @author Tomas Kral

#include "odblib/DataSelectOutput.h"
#include "odblib/DataSelectIterator.h"

using namespace odb;

namespace odb {
namespace internal {

DataSelectOutput::DataSelectOutput(DataSelectIterator& it)
  : it_(it),
    count_(0)
{}

DataSelectOutput::~DataSelectOutput()
{}

bool DataSelectOutput::output(const Expressions& results)
{
    double* const data = it_.data();
    size_t size = results.size();
    bool missing = false;

    for (size_t i = 0; i < size; i++)
        data[i] = results[i]->eval(missing /*= false */);

    ++count_;

    return true;
}

} // namespace internal
} // namespace odb
