#ifndef _MASTERCORE_TRADE_TOKENS
#define _MASTERCORE_TRADE_TOKENS

#include "mastercore_txs/serialize.h"

#include <stdint.h>
#include <string>

#include "tinyformat.h"

namespace mastercore {
namespace transaction {

class CTemplateTradeTokens
{
public:
    uint16_t version;
    uint16_t type;
    uint32_t property;
    int64_t amount_for_sale;
    uint32_t property_desired;
    int64_t amount_desired;
    uint8_t subaction;

    CTemplateTradeTokens()
    {
        SetNull();
    };

    CTemplateTradeTokens(
        uint32_t property_in,
        int64_t amount_for_sale_in,
        uint32_t property_desired_in,
        int64_t amount_desired_in,
        uint8_t subaction_in)
    {
        version = 0;
        type = 21;
        property = property_in;
        amount_for_sale = amount_for_sale_in;
        property_desired = property_desired_in;
        amount_desired = amount_desired_in;
        subaction = subaction_in;
    };

    void SetNull()
    {
        version = 0;
        type = 21;
        property = 0;
        amount_for_sale = 0;
        property_desired = 0;
        amount_desired = 0;
        subaction = 0;
    };

    CAN_BE_SERIALIZED
    (
        SERIALIZE_16(version);
        SERIALIZE_16(type);
        SERIALIZE_32(property);
        SERIALIZE_64(amount_for_sale);
        SERIALIZE_32(property_desired);
        SERIALIZE_64(amount_desired);
        SERIALIZE_8(subaction);
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE_16(version);
        DESERIALIZE_16(type);
        DESERIALIZE_32(property);
        DESERIALIZE_64(amount_for_sale);
        DESERIALIZE_32(property_desired);        
        DESERIALIZE_64(amount_desired);
        DESERIALIZE_8(subaction);
    );

    CAN_BE_PUSHED
    (
        PUSH_JSON(version);
        PUSH_JSON(type);
        PUSH_JSON(property);
        PUSH_JSON(amount_for_sale);
        PUSH_JSON(property_desired);
        PUSH_JSON(amount_desired);
        PUSH_JSON(subaction);
    );

    STATIC_DESERIALIZATION(CTemplateTradeTokens);
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_TRADE_TOKENS
