/// @file   DataField.cc
/// @author Tomas Kral

#include "odblib/DataField.h"

#include <ios>
#include <iostream>

using namespace std;

namespace odb {

DataField::DataField(DataRow& row, const DataColumn& column, size_t index)
  : row_(&row),
    column_(&column),
    index_(index)
{}

std::ostream& operator<<(std::ostream& stream, const DataField& field)
{
    stream << field.name() << ": ";

    switch (field.type())
    {
        case INTEGER: stream << field.as<long>();
        case REAL:    stream << field.as<float>();
        case DOUBLE:  stream << field.as<double>();
        case STRING:  stream << field.as<string>();
        case BITFIELD:
            stream << hex << setiosflags(ios_base::showbase);
            stream << field.as<unsigned int>();
            stream << dec << resetiosflags(ios_base::showbase);
            break;
            
        default: ASSERT(!"Unknown DataType!");
    }

    return stream;
}

} // namespace odb
