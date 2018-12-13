#include "odc/CodecFactory.h"
#include "odc/Codec.h"

namespace odc {
namespace codec {

void CodecFactoryBase::loadCodecs()
{
    Codec::loadCodecs();

}

} // codec
} // odb
