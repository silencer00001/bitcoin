#ifndef _MASTERCORE_OFFER_TOKENS_V1
#define _MASTERCORE_OFFER_TOKENS_V1

#include "mastercore_txs/serialize.h"
#include "mastercore_txs/template_base.h"

#include <stdint.h>

namespace mastercore {
namespace transaction {

class CTemplateOfferTokensV1: public CTemplateBase
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
        SERIALIZE(version);
        SERIALIZE(type);
        SERIALIZE(property);
        SERIALIZE(amount_for_sale);
        SERIALIZE(amount_desired);
        SERIALIZE(block_time_limit);
        SERIALIZE(commitment_fee);
        SERIALIZE(subaction);
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE(version);
        DESERIALIZE(type);
        DESERIALIZE(property);
        DESERIALIZE(amount_for_sale);
        DESERIALIZE(amount_desired);
        DESERIALIZE(block_time_limit);
        DESERIALIZE(commitment_fee);
        DESERIALIZE(subaction);
    );

    CAN_BE_SERIALIZED_WITH_LABEL
    (
        SERIALIZE_WITH_LABEL(version);
        SERIALIZE_WITH_LABEL(type);
        SERIALIZE_WITH_LABEL(property);
        SERIALIZE_WITH_LABEL(amount_for_sale);
        SERIALIZE_WITH_LABEL(amount_desired);
        SERIALIZE_WITH_LABEL(block_time_limit);
        SERIALIZE_WITH_LABEL(commitment_fee);
        SERIALIZE_WITH_LABEL(subaction);
    );
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_OFFER_TOKENS_V1
