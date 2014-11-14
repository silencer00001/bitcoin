#ifndef _MASTERCORE_CREATE_CROWDSALE
#define _MASTERCORE_CREATE_CROWDSALE

#include "mastercore_txs/serialize.h"
#include "mastercore_txs/template_base.h"

#include <stdint.h>
#include <string>

namespace mastercore {
namespace transaction {

class CTemplateCreateCrowdsale: public CTemplateBase
{
public:
    uint16_t version;
    uint16_t type;
    uint8_t ecosystem;
    uint16_t property_type;
    uint32_t previous_property_id;
    std::string category;
    std::string subcategory;
    std::string name;
    std::string url;
    std::string data;
    uint32_t property_desired;
    int64_t token_per_unit_vested;
    uint64_t deadline;
    uint8_t early_bird_bonus;
    uint8_t issuer_bonus;

    CTemplateCreateCrowdsale()
    {
        SetNull();
    };

    CTemplateCreateCrowdsale(
        uint8_t ecosystem_in,
        uint16_t property_type_in,
        uint32_t previous_property_id_in,
        std::string category_in,
        std::string subcategory_in,
        std::string name_in,
        std::string url_in,
        std::string data_in,
        uint32_t property_desired_in,
        int64_t token_per_unit_vested_in,  
        uint64_t deadline_in,
        uint8_t early_bird_bonus_in,
        uint8_t issuer_bonus_in)
    {
        version = 0;
        type = 51;
        ecosystem = ecosystem_in;
        property_type = property_type_in;
        previous_property_id = previous_property_id_in;
        category = category_in;
        subcategory = subcategory_in;
        name = name_in;
        url = url_in;
        data = data_in;
        property_desired = property_desired_in;
        token_per_unit_vested = token_per_unit_vested_in;
        deadline = deadline_in;
        early_bird_bonus = early_bird_bonus_in;
        issuer_bonus = issuer_bonus_in;
    };

    void SetNull()
    {
        version = 0;
        type = 51;
        ecosystem = 0;
        property_type = 0;
        previous_property_id = 0;
        category.clear();
        subcategory.clear();
        name.clear();
        url.clear();
        data.clear();
        property_desired = 0;
        token_per_unit_vested = 0;
        deadline = 0;
        early_bird_bonus = 0;
        issuer_bonus = 0;
    };

    CAN_BE_SERIALIZED
    (
        SERIALIZE(version);
        SERIALIZE(type);
        SERIALIZE(ecosystem);
        SERIALIZE(property_type);
        SERIALIZE(previous_property_id);
        SERIALIZE(category);
        SERIALIZE(subcategory);
        SERIALIZE(name);
        SERIALIZE(url);
        SERIALIZE(data);
        SERIALIZE(property_desired);
        SERIALIZE(token_per_unit_vested);       
        SERIALIZE(deadline);
        SERIALIZE(early_bird_bonus);
        SERIALIZE(issuer_bonus);
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE(version);
        DESERIALIZE(type);
        DESERIALIZE(ecosystem);
        DESERIALIZE(property_type);
        DESERIALIZE(previous_property_id);
        DESERIALIZE(category);
        DESERIALIZE(subcategory);
        DESERIALIZE(name);
        DESERIALIZE(url);
        DESERIALIZE(data);
        DESERIALIZE(property_desired);
        DESERIALIZE(token_per_unit_vested);       
        DESERIALIZE(deadline);
        DESERIALIZE(early_bird_bonus);
        DESERIALIZE(issuer_bonus);
    );

    CAN_BE_SERIALIZED_WITH_LABEL
    (
        SERIALIZE_WITH_LABEL(version);
        SERIALIZE_WITH_LABEL(type);
        SERIALIZE_WITH_LABEL(ecosystem);
        SERIALIZE_WITH_LABEL(property_type);
        SERIALIZE_WITH_LABEL(previous_property_id);
        SERIALIZE_WITH_LABEL(category);
        SERIALIZE_WITH_LABEL(subcategory);
        SERIALIZE_WITH_LABEL(name);
        SERIALIZE_WITH_LABEL(url);
        SERIALIZE_WITH_LABEL(data);
        SERIALIZE_WITH_LABEL(property_desired);
        SERIALIZE_WITH_LABEL(token_per_unit_vested);
        SERIALIZE_WITH_LABEL(deadline);
        SERIALIZE_WITH_LABEL(early_bird_bonus);
        SERIALIZE_WITH_LABEL(issuer_bonus);
    );
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_CREATE_CROWDSALE
