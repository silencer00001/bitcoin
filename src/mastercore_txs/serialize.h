#ifndef _MASTERCORE_SERIALIZE
#define _MASTERCORE_SERIALIZE

#include <endian.h>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

#include "json/json_spirit_value.h"

namespace mastercore {
namespace transaction {

#define PUSH_BACK_BYTES(vector, value)                                  \
    vector.insert(vector.end(),                                         \
        reinterpret_cast<unsigned char *>(&(value)),                    \
        reinterpret_cast<unsigned char *>(&(value)) + sizeof((value)));

#define PUSH_BACK_BYTES_PTR(vector, ptr, size)                          \
    vector.insert(vector.end(),                                         \
        reinterpret_cast<unsigned char *>((ptr)),                       \
        reinterpret_cast<unsigned char *>((ptr)) + (size));

#define SERIALIZE_8(value)                                              \
    {                                                                   \
        nSerializedSize += 1;                                           \
        PUSH_BACK_BYTES(_vch, value);                                   \
    }

#define SERIALIZE_16(value)                                             \
    {                                                                   \
        nSerializedSize += 2;                                           \
        uint16_t _##value = htobe16(value);                             \
        PUSH_BACK_BYTES(_vch, _##value);                                \
    }

#define SERIALIZE_32(value)                                             \
    {                                                                   \
        nSerializedSize += 4;                                           \
        uint32_t _##value = htobe32(value);                             \
        PUSH_BACK_BYTES(_vch, _##value);                                \
    }

#define SERIALIZE_64(value)                                             \
    {                                                                   \
        nSerializedSize += 8;                                           \
        uint64_t _##value = htobe64(value);                             \
        PUSH_BACK_BYTES(_vch, _##value);                                \
    }

#define SERIALIZE_STRING(value)                                         \
    {                                                                   \
        for (size_t i = 0; i < (value).size(); ++i) {                   \
            nSerializedSize++;                                          \
            uint8_t c = static_cast<uint8_t>((value).at(i));            \
            PUSH_BACK_BYTES(_vch, c);                                   \
        }                                                               \
        nSerializedSize += 1;                                           \
        uint8_t _##value = 0;                                           \
        PUSH_BACK_BYTES(_vch, _##value);                                \
    }

#define DESERIALIZE_8(value)                                            \
    if (_vch.size() < _pos + 1) { return false; }                       \
    value = *reinterpret_cast<const uint8_t*>(&_vch[_pos]);             \
    _pos += 1;

#define DESERIALIZE_16(value)                                           \
    if (_vch.size() < _pos + 2) { return false; }                       \
    value = be16toh(*reinterpret_cast<const uint16_t*>(&_vch[_pos]));   \
    _pos += 2;

#define DESERIALIZE_32(value)                                           \
    if (_vch.size() < _pos + 4) { return false; }                       \
    value = be32toh(*reinterpret_cast<const uint32_t*>(&_vch[_pos]));   \
    _pos += 4;

#define DESERIALIZE_64(value)                                           \
    if (_vch.size() < _pos + 8) { return false; }                       \
    value = be64toh(*reinterpret_cast<const uint64_t*>(&_vch[_pos]));   \
    _pos += 8;

#define DESERIALIZE_STRING(value)                                       \
    if (_vch.size() < _pos + 1) { return false; }                       \
    {                                                                   \
        std::stringstream ss;                                           \
        uint8_t c = *reinterpret_cast<const uint8_t*>(&_vch[_pos]);     \
        _pos += 1;                                                      \
        while (c != 0) {                                                \
            ss << c;                                                    \
            if (_vch.size() < _pos + 1) { return false; }               \
            c = *reinterpret_cast<const uint8_t*>(&_vch[_pos]);         \
            _pos += 1;                                                  \
        };                                                              \
        value = ss.str();                                               \
    }

#define CAN_BE_SERIALIZED(statements)                                   \
    size_t nSerializedSize;                                             \
    std::vector<unsigned char> Serialize()                              \
    {                                                                   \
        std::vector<unsigned char> _vch;                                \
        {statements}                                                    \
        return _vch;                                                    \
    }

#define CAN_BE_DESERIALIZED(statements)                                 \
    size_t _pos;                                                        \
    bool Deserialize(const std::vector<unsigned char>& _vch)            \
    {                                                                   \
        _pos = 0;                                                       \
        {statements}                                                    \
        return true;                                                    \
    }

#define STATIC_DESERIALIZATION(T)                                       \
    static T Parse(const std::vector<unsigned char>& _vch)              \
    {                                                                   \
        T _obj = T();                                                   \
        _obj.Deserialize(_vch);                                         \
        return _obj;                                                    \
    }

#define PUSH_JSON(value)                                                \
    {                                                                   \
        _obj.push_back(json_spirit::Pair(#value, (uint64_t)value));     \
    }

#define PUSH_JSON_STR(value)                                            \
    {                                                                   \
        _obj.push_back(json_spirit::Pair(#value, value));               \
    }

#define CAN_BE_PUSHED(statements)                                       \
    void ToJSON(json_spirit::Object& _obj)                              \
    {                                                                   \
        {statements}                                                    \
    }

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_SERIALIZE
