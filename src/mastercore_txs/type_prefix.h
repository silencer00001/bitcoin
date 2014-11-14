#ifndef _MASTERCORE_TYPE_PREFIX
#define _MASTERCORE_TYPE_PREFIX

#include "mastercore_txs/serialize.h"
#include "mastercore_txs/template_base.h"

#include <stdint.h>

namespace mastercore {
namespace transaction {

class CTemplateTypePrefix: public CTemplateBase
{
public:
    uint16_t version;
    uint16_t type;

    CTemplateTypePrefix()
    {
        SetNull();
    };

    CTemplateTypePrefix(uint32_t version_in, int64_t type_in)
    {
        version = version_in;
        type = type_in;
    };

    void SetNull()
    {
        version = UINT16_MAX;
        type = UINT16_MAX;
    };

    CAN_BE_SERIALIZED
    (
        SERIALIZE(version);
        SERIALIZE(type);
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE(version);
        DESERIALIZE(type);
    );

    CAN_BE_SERIALIZED_WITH_LABEL
    (
        SERIALIZE_WITH_LABEL(version);
        SERIALIZE_WITH_LABEL(type);
    );
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_TYPE_PREFIX
