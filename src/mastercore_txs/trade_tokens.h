#ifndef _MASTERCORE_TRADE_TOKENS
#define _MASTERCORE_TRADE_TOKENS

#include "mastercore_txs/serialize.h"
#include "mastercore_txs/template_base.h"

#include <stdint.h>

namespace mastercore {
namespace transaction {

class CTemplateTradeTokens: public CTemplateBase
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
        SERIALIZE(version);
        SERIALIZE(type);
        SERIALIZE(property);
        SERIALIZE(amount_for_sale);
        SERIALIZE(property_desired);
        SERIALIZE(amount_desired);
        SERIALIZE(subaction);
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE(version);
        DESERIALIZE(type);
        DESERIALIZE(property);
        DESERIALIZE(amount_for_sale);
        DESERIALIZE(property_desired);        
        DESERIALIZE(amount_desired);
        DESERIALIZE(subaction);
    );

    CAN_BE_SERIALIZED_WITH_LABEL
    (
        SERIALIZE_WITH_LABEL(version);
        SERIALIZE_WITH_LABEL(type);
        SERIALIZE_WITH_LABEL(property);
        SERIALIZE_WITH_LABEL(amount_for_sale);
        SERIALIZE_WITH_LABEL(property_desired);
        SERIALIZE_WITH_LABEL(amount_desired);
        SERIALIZE_WITH_LABEL(subaction);
    );
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_TRADE_TOKENS
