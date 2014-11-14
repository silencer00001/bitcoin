#ifndef _MASTERCORE_REVOKE_TOKEN
#define _MASTERCORE_REVOKE_TOKEN

#include "mastercore_txs/serialize.h"
#include "mastercore_txs/template_base.h"

#include <stdint.h>

namespace mastercore {
namespace transaction {

class CTemplateRevokeToken: public CTemplateBase
{
public:
    uint16_t version;
    uint16_t type;
    uint32_t property;
    int64_t amount;
    std::string memo;

    CTemplateRevokeToken()
    {
        SetNull();
    };

    CTemplateRevokeToken(uint32_t property_in, int64_t amount_in, std::string memo_in = "")
    {
        version = 0;
        type = 56;
        property = property_in;
        amount = amount_in;
        memo = memo_in;
    };

    void SetNull()
    {
        version = 0;
        type = 56;
        property = 0;
        amount = 0;
        memo.clear();
    };

    CAN_BE_SERIALIZED
    (
        SERIALIZE(version);
        SERIALIZE(type);
        SERIALIZE(property);
        SERIALIZE(amount);
        if (memo.size() > 0) { SERIALIZE(memo); }  // optional
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE(version);
        DESERIALIZE(type);
        DESERIALIZE(property);
        DESERIALIZE(amount);
        if (vch.size() > pos) { DESERIALIZE(memo); }  // optional
    );

    CAN_BE_SERIALIZED_WITH_LABEL
    (
        SERIALIZE_WITH_LABEL(version);
        SERIALIZE_WITH_LABEL(type);
        SERIALIZE_WITH_LABEL(property);
        SERIALIZE_WITH_LABEL(amount);
        if (memo.size() > 0) { SERIALIZE_WITH_LABEL(memo); }  // optional
    );
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_REVOKE_TOKEN
