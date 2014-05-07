#include "odb_api/odblib/CodecFactory.h"
#include "odb_api/odblib/Codec.h"

namespace odb {
namespace codec {

void CodecFactoryBase::loadCodecs()
{
    Codec::loadCodecs();

}

} // codec
} // odb
