#ifndef _MASTERCORE_SEND_TO_OWNERS
#define _MASTERCORE_SEND_TO_OWNERS

#include "mastercore_txs/serialize.h"

#include <stdint.h>
#include <string>

#include "tinyformat.h"

namespace mastercore {
namespace transaction {

class CTemplateSendToOwners
{
public:
    uint16_t version;
    uint16_t type;
    uint32_t property;
    int64_t amount;

    CTemplateSendToOwners()
    {
        SetNull();
    };

    CTemplateSendToOwners(uint32_t property_in, int64_t amount_in)
    {
        version = 0;
        type = 3;
        property = property_in;
        amount = amount_in;
    };

    void SetNull()
    {
        version = 0;
        type = 3;
        property = 0;
        amount = 0;
    };

    CAN_BE_SERIALIZED
    (
        SERIALIZE_16(version);
        SERIALIZE_16(type);
        SERIALIZE_32(property);
        SERIALIZE_64(amount);
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE_16(version);
        DESERIALIZE_16(type);
        DESERIALIZE_32(property);
        DESERIALIZE_64(amount);
    );

    CAN_BE_PUSHED
    (
        PUSH_JSON(version);
        PUSH_JSON(type);
        PUSH_JSON(property);
        PUSH_JSON(amount);
    );

    STATIC_DESERIALIZATION(CTemplateSendToOwners);
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_SEND_TO_OWNERS
