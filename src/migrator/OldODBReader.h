#ifndef OldODBReader_H
#define OldODBReader_H

#include "TSQLReader.h"
#include "ODBIterator.h"

namespace odb {
namespace tool {

typedef odb::tool::TSQLReader<ODBIterator> OldODBReader;

} // namespace tool 
} // namespace odb 

#endif
