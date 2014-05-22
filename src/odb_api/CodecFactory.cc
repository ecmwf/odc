#include "odb_api/CodecFactory.h"
#include "odb_api/Codec.h"

namespace odb {
namespace codec {

void CodecFactoryBase::loadCodecs()
{
    Codec::loadCodecs();

}

} // codec
} // odb
