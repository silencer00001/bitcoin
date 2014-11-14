#ifndef _MASTERCORE_OFFER_TOKENS_V0
#define _MASTERCORE_OFFER_TOKENS_V0

#include "mastercore_txs/serialize.h"

#include <stdint.h>
#include <string>

#include "tinyformat.h"

namespace mastercore {
namespace transaction {

class CTemplateOfferTokensV0
{
public:
    uint16_t version;
    uint16_t type;
    uint32_t property;
    int64_t amount_for_sale;    
    int64_t amount_desired;
    uint8_t block_time_limit;
    int64_t commitment_fee;

    CTemplateOfferTokensV0()
    {
        SetNull();
    };

    CTemplateOfferTokensV0(
        uint32_t property_in,
        int64_t amount_for_sale_in,
        int64_t amount_desired_in,
        uint8_t block_time_limit_in,
        int64_t commitment_fee_in)
    {
        version = 0;
        type = 20;
        property = property_in;
        amount_for_sale = amount_for_sale_in;
        amount_desired = amount_desired_in;
        block_time_limit = block_time_limit_in;
        commitment_fee = commitment_fee_in;
    };

    void SetNull()
    {
        version = 0;
        type = 20;
        property = 0;
        amount_for_sale = 0;
        amount_desired = 0;
        block_time_limit = 0;
        commitment_fee = 0;
    };

    CAN_BE_SERIALIZED
    (
        SERIALIZE_16(version);
        SERIALIZE_16(type);
        SERIALIZE_32(property);
        SERIALIZE_64(amount_for_sale);
        SERIALIZE_64(amount_desired);
        SERIALIZE_8(block_time_limit);
        SERIALIZE_64(commitment_fee);
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE_16(version);
        DESERIALIZE_16(type);
        DESERIALIZE_32(property);
        DESERIALIZE_64(amount_for_sale);
        DESERIALIZE_64(amount_desired);
        DESERIALIZE_8(block_time_limit);
        DESERIALIZE_64(commitment_fee);
    );

    CAN_BE_PUSHED
    (
        PUSH_JSON(version);
        PUSH_JSON(type);
        PUSH_JSON(property);
        PUSH_JSON(amount_for_sale);
        PUSH_JSON(amount_desired);
        PUSH_JSON(block_time_limit);
        PUSH_JSON(commitment_fee);
    );

    STATIC_DESERIALIZATION(CTemplateOfferTokensV0);
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_OFFER_TOKENS_V0
