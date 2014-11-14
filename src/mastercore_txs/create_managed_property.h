#ifndef _MASTERCORE_CREATE_MANAGED_PROPERTY
#define _MASTERCORE_CREATE_MANAGED_PROPERTY

#include "mastercore_txs/serialize.h"

#include <stdint.h>
#include <string>

#include "tinyformat.h"

namespace mastercore {
namespace transaction {

class CTemplateCreateManagedProperty
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
    
    CTemplateCreateManagedProperty()
    {
        SetNull();
    };

    CTemplateCreateManagedProperty(
        uint8_t ecosystem_in,
        uint16_t property_type_in,
        uint32_t previous_property_id_in,
        std::string category_in,
        std::string subcategory_in,
        std::string name_in,
        std::string url_in,
        std::string data_in)
    {
        version = 0;
        type = 54;
        ecosystem = ecosystem_in;
        property_type = property_type_in;
        previous_property_id = previous_property_id_in;
        category = category_in;
        subcategory = subcategory_in;
        name = name_in;
        url = url_in;
        data = data_in;
    };

    void SetNull()
    {
        version = 0;
        type = 54;
        ecosystem = 0;
        property_type = 0;
        previous_property_id = 0;
        category.clear();
        subcategory.clear();
        name.clear();
        url.clear();
        data.clear();
    };

    CAN_BE_SERIALIZED
    (
        SERIALIZE_16(version);
        SERIALIZE_16(type);
        SERIALIZE_8(ecosystem);
        SERIALIZE_16(property_type);
        SERIALIZE_32(previous_property_id);
        SERIALIZE_STRING(category);
        SERIALIZE_STRING(subcategory);
        SERIALIZE_STRING(name);
        SERIALIZE_STRING(url);
        SERIALIZE_STRING(data);
    );

    CAN_BE_DESERIALIZED
    (
        DESERIALIZE_16(version);
        DESERIALIZE_16(type);
        DESERIALIZE_8(ecosystem);
        DESERIALIZE_16(property_type);
        DESERIALIZE_32(previous_property_id);
        DESERIALIZE_STRING(category);
        DESERIALIZE_STRING(subcategory);
        DESERIALIZE_STRING(name);
        DESERIALIZE_STRING(url);
        DESERIALIZE_STRING(data);
    );

    CAN_BE_PUSHED
    (
        PUSH_JSON(version);
        PUSH_JSON(type);
        PUSH_JSON(ecosystem);
        PUSH_JSON(property_type);
        PUSH_JSON(previous_property_id);
        PUSH_JSON_STR(category);
        PUSH_JSON_STR(subcategory);
        PUSH_JSON_STR(name);
        PUSH_JSON_STR(url);
        PUSH_JSON_STR(data);
    );

    STATIC_DESERIALIZATION(CTemplateCreateManagedProperty);
};

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_CREATE_MANAGED_PROPERTY
