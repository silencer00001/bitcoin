#ifndef _MASTERCORE_OFFER_TOKENS_V0
#define _MASTERCORE_OFFER_TOKENS_V0

#include "mastercore_txs/serialize.h"
#include "mastercore_txs/template_base.h"

#include <stdint.h>

namespace mastercore {
namespace transaction {

class CTemplateOfferTokensV0: public CTemplateBase
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
        SERIALIZE(version);
        SERIALIZE(type);
        SERIALIZE(property);
        SERIALIZE(amount_for_sale);
        SERIALIZE(amount_desired);
        SERIALIZE(block_time_limit);
        SERIALIZE(commitment_fee);
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
    );
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_OFFER_TOKENS_V0
