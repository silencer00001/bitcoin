#ifndef _MASTERCORE_GRANT_TOKEN
#define _MASTERCORE_GRANT_TOKEN

#include "mastercore_txs/serialize.h"

#include <stdint.h>
#include <string>

#include "tinyformat.h"

namespace mastercore {
namespace transaction {

class CTemplateGrantToken
{
public:
    uint16_t version;
    uint16_t type;
    uint32_t property;
    int64_t amount;
    std::string memo;

    CTemplateGrantToken()
    {
        SetNull();
    };

    CTemplateGrantToken(uint32_t property_in, int64_t amount_in, std::string memo_in)
    {
        version = 0;
        type = 55;
        property = property_in;
        amount = amount_in;
        memo = memo_in;
    };

    void SetNull()
    {
        version = 0;
        type = 55;
        property = 0;
        amount = 0;
        memo.clear();
    };

    CAN_BE_SERIALIZED
    (
        SERIALIZE_16(version);
        SERIALIZE_16(type);
        SERIALIZE_32(property);
        SERIALIZE_64(amount);
        if (memo.size() > 0) { SERIALIZE_STRING(memo); }  // optional
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE_16(version);
        DESERIALIZE_16(type);
        DESERIALIZE_32(property);
        DESERIALIZE_64(amount);
        if (_vch.size() > _pos) { DESERIALIZE_STRING(memo); }  // optional
    );

    CAN_BE_PUSHED
    (
        PUSH_JSON(version);
        PUSH_JSON(type);
        PUSH_JSON(property);
        PUSH_JSON(amount);
        if (memo.size() > 0) { PUSH_JSON_STR(memo); }  // optional
    );

    STATIC_DESERIALIZATION(CTemplateGrantToken);
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_GRANT_TOKEN
