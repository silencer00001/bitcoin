#ifndef _MASTERCORE_SIMPLE_SEND
#define _MASTERCORE_SIMPLE_SEND

#include "mastercore_txs/serialize.h"
#include "mastercore_txs/template_base.h"

#include <stdint.h>

namespace mastercore {
namespace transaction {

class CTemplateSimpleSend: public CTemplateBase
{
public:
    uint16_t version;
    uint16_t type;
    uint32_t property;
    int64_t amount;

    CTemplateSimpleSend()
    {
        SetNull();
    };

    CTemplateSimpleSend(uint32_t property_in, int64_t amount_in)
    {
        version = 0;
        type = 0;
        property = property_in;
        amount = amount_in;
    };

    void SetNull()
    {
        version = 0;
        type = 0;
        property = 0;
        amount = 0;
    };

    CAN_BE_SERIALIZED
    (
        SERIALIZE(version);
        SERIALIZE(type);
        SERIALIZE(property);
        SERIALIZE(amount);
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE(version);
        DESERIALIZE(type);
        DESERIALIZE(property);
        DESERIALIZE(amount);
    );

    CAN_BE_SERIALIZED_WITH_LABEL
    (
        SERIALIZE_WITH_LABEL(version);
        SERIALIZE_WITH_LABEL(type);
        SERIALIZE_WITH_LABEL(property);
        SERIALIZE_WITH_LABEL(amount);
    );
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_SIMPLE_SEND
