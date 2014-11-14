#ifndef _MASTERCORE_OFFER_TOKENS_V1
#define _MASTERCORE_OFFER_TOKENS_V1

#include "mastercore_txs/serialize.h"

#include <stdint.h>
#include <string>

#include "tinyformat.h"

namespace mastercore {
namespace transaction {

class CTemplateOfferTokensV1
{
public:
    uint16_t version;
    uint16_t type;
    uint32_t property;
    int64_t amount_for_sale;    
    int64_t amount_desired;
    uint8_t block_time_limit;
    int64_t commitment_fee;
    uint8_t subaction;

    CTemplateOfferTokensV1()
    {
        SetNull();
    };

    CTemplateOfferTokensV1(
        uint32_t property_in,
        int64_t amount_for_sale_in,
        int64_t amount_desired_in,
        uint8_t block_time_limit_in,
        int64_t commitment_fee_in,
        uint8_t subaction_in)
    {
        version = 1;
        type = 20;
        property = property_in;
        amount_for_sale = amount_for_sale_in;
        amount_desired = amount_desired_in;
        block_time_limit = block_time_limit_in;
        commitment_fee = commitment_fee_in;
        subaction = subaction_in;
    };

    void SetNull()
    {
        version = 1;
        type = 20;
        property = 0;
        amount_for_sale = 0;
        amount_desired = 0;
        block_time_limit = 0;
        commitment_fee = 0;
        subaction = 0;
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
        SERIALIZE_8(subaction);
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
        DESERIALIZE_8(subaction);
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
        PUSH_JSON(subaction);
    );

    STATIC_DESERIALIZATION(CTemplateOfferTokensV1);
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_OFFER_TOKENS_V1
