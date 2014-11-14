#ifndef _MASTERCORE_CREATE_PROPERTY
#define _MASTERCORE_CREATE_PROPERTY

#include "mastercore_txs/serialize.h"
#include "mastercore_txs/template_base.h"

#include <stdint.h>
#include <string>

namespace mastercore {
namespace transaction {

class CTemplateCreateProperty: public CTemplateBase
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
    int64_t amount;

    CTemplateCreateProperty()
    {
        SetNull();
    };

    CTemplateCreateProperty(
        uint8_t ecosystem_in,
        uint16_t property_type_in,
        uint32_t previous_property_id_in,
        std::string category_in,
        std::string subcategory_in,
        std::string name_in,
        std::string url_in,
        std::string data_in,
        int64_t amount_in)
    {
        version = 0;
        type = 50;
        ecosystem = ecosystem_in;
        property_type = property_type_in;
        previous_property_id = previous_property_id_in;
        category = category_in;
        subcategory = subcategory_in;
        name = name_in;
        url = url_in;
        data = data_in;
        amount = amount_in;
    };

    void SetNull()
    {
        version = 0;
        type = 50;
        ecosystem = 0;
        property_type = 0;
        previous_property_id = 0;
        category.clear();
        subcategory.clear();
        name.clear();
        url.clear();
        data.clear();
        amount = 0;
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
        SERIALIZE(amount);
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
        DESERIALIZE(amount);
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
        SERIALIZE_WITH_LABEL(amount);
    );
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_CREATE_PROPERTY
