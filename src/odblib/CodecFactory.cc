#include "odblib/CodecFactory.h"
#include "odblib/Codec.h"

namespace odb {
namespace codec {

void CodecFactoryBase::loadCodecs()
{
    Codec::loadCodecs();

}

} // codec
} // odb
