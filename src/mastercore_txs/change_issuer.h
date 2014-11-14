#ifndef MASTERCORE_TRANSACTIONS_CHANGE_ISSUER_H
#define MASTERCORE_TRANSACTIONS_CHANGE_ISSUER_H

#include "mastercore_txs/serialize.h"
#include "mastercore_txs/template_base.h"

#include <stdint.h>

namespace mastercore {
namespace transaction {

class CTemplateChangeIssuer: public CTemplateBase
{
public:
    uint16_t version;
    uint16_t type;
    uint32_t property;

    CTemplateChangeIssuer()
    {
        SetNull();
    };

    CTemplateChangeIssuer(uint32_t property_in)
    {
        version = 0;
        type = 70;
        property = property_in;
    };

    void SetNull()
    {
        version = 0;
        type = 70;
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

#endif // MASTERCORE_TRANSACTIONS_CHANGE_ISSUER_H
