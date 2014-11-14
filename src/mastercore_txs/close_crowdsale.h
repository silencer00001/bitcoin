#ifndef _MASTERCORE_CLOSE_CROWDSALE
#define _MASTERCORE_CLOSE_CROWDSALE

#include "mastercore_txs/serialize.h"
#include "mastercore_txs/template_base.h"

#include <stdint.h>

namespace mastercore {
namespace transaction {

class CTemplateCloseCrowdsale: public CTemplateBase
{
public:
    uint16_t version;
    uint16_t type;
    uint32_t property;

    CTemplateCloseCrowdsale()
    {
        SetNull();
    };

    CTemplateCloseCrowdsale(uint32_t property_in)
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
        SERIALIZE(version);
        SERIALIZE(type);
        SERIALIZE(property);
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE(version);
        DESERIALIZE(type);
        DESERIALIZE(property);
    );

    CAN_BE_SERIALIZED_WITH_LABEL
    (
        SERIALIZE_WITH_LABEL(version);
        SERIALIZE_WITH_LABEL(type);
        SERIALIZE_WITH_LABEL(property);
    );
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_CLOSE_CROWDSALE
