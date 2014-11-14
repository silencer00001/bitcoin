#ifndef _MASTERCORE_CLOSE_CROWDSALE
#define _MASTERCORE_CLOSE_CROWDSALE

#include "mastercore_txs/serialize.h"

#include <stdint.h>
#include <string>

#include "tinyformat.h"

namespace mastercore {
namespace transaction {

class CTemplateCloseCrowdsale
{
public:
    uint16_t version;
    uint16_t type;
    uint32_t property;

    CTemplateCloseCrowdsale()
    {
        SetNull();
    };

    CTemplateCloseCrowdsale(uint32_t property_in, int64_t amount_in)
    {
        version = 0;
        type = 53;
        property = property_in;
    };

    void SetNull()
    {
        version = 0;
        type = 53;
        property = 0;
    };

    CAN_BE_SERIALIZED
    (
        SERIALIZE_16(version);
        SERIALIZE_16(type);
        SERIALIZE_32(property);
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE_16(version);
        DESERIALIZE_16(type);
        DESERIALIZE_32(property);
    );

    CAN_BE_PUSHED
    (
        PUSH_JSON(version);
        PUSH_JSON(type);
        PUSH_JSON(property);
    );

    STATIC_DESERIALIZATION(CTemplateCloseCrowdsale);
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_CLOSE_CROWDSALE
