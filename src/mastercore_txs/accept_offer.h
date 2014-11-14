#ifndef _MASTERCORE_ACCEPT_OFFER
#define _MASTERCORE_ACCEPT_OFFER

#include "mastercore_txs/serialize.h"

#include <stdint.h>
#include <string>

#include "tinyformat.h"

namespace mastercore {
namespace transaction {

class CTemplateAcceptOffer
{
public:
    uint16_t version;
    uint16_t type;
    uint32_t property;
    int64_t amount;

    CTemplateAcceptOffer()
    {
        SetNull();
    };

    CTemplateAcceptOffer(uint32_t property_in, int64_t amount_in)
    {
        version = 0;
        type = 22;
        property = property_in;
        amount = amount_in;
    };

    void SetNull()
    {
        version = 0;
        type = 22;
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

    STATIC_DESERIALIZATION(CTemplateAcceptOffer);
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_ACCEPT_OFFER
