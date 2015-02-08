#ifndef _MASTERCORE_SERIALIZE
#define _MASTERCORE_SERIALIZE

#include <endian.h>
#include <stdint.h>

#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "json/json_spirit_value.h"

namespace mastercore {
namespace transaction {

#define SERIALIZE(value)             write(vch, value)
#define SERIALIZE_WITH_LABEL(value)  write(obj, value, #value)
#define DESERIALIZE(value)           read(vch, value, pos)


#define CAN_BE_SERIALIZED(statements)                                   \
    std::vector<unsigned char> Serialize() const                        \
    {                                                                   \
        std::vector<unsigned char> vch;                                 \
        {statements}                                                    \
        return vch;                                                     \
    }                                                                   \
    void Serialize(std::vector<unsigned char>& vch) const               \
    {                                                                   \
        {statements}                                                    \
    }                                                                   \

#define CAN_BE_SERIALIZED_WITH_LABEL(statements)                        \
    void Serialize(json_spirit::Object& obj) const                      \
    {                                                                   \
        {statements}                                                    \
    }

#define CAN_BE_DESERIALIZED(statements)                                 \
    bool Deserialize(const std::vector<unsigned char>& vch)             \
    {                                                                   \
        size_t pos = 0;                                                 \
        {statements}                                                    \
        return true;                                                    \
    }

    inline int8_t   htobe(int8_t   n) { return n; }
    inline uint8_t  htobe(uint8_t  n) { return n; }
    inline int16_t  htobe(int16_t  n) { return htobe16(n); }
    inline uint16_t htobe(uint16_t n) { return htobe16(n); }
    inline int32_t  htobe(int32_t  n) { return htobe32(n); }
    inline uint32_t htobe(uint32_t n) { return htobe32(n); }
    inline int64_t  htobe(int64_t  n) { return htobe64(n); }
    inline uint64_t htobe(uint64_t n) { return htobe64(n); }    
    inline int8_t   betoh(int8_t   n) { return n; }
    inline uint8_t  betoh(uint8_t  n) { return n; }
    inline int16_t  betoh(int16_t  n) { return be16toh(n); }
    inline uint16_t betoh(uint16_t n) { return be16toh(n); }
    inline int32_t  betoh(int32_t  n) { return be32toh(n); }
    inline uint32_t betoh(uint32_t n) { return be32toh(n); }
    inline int64_t  betoh(int64_t  n) { return be64toh(n); }
    inline uint64_t betoh(uint64_t n) { return be64toh(n); }


    
    template<typename T>
    inline void write(std::vector<unsigned char>& vch, T value)
    {
        T val = htobe(value);
        vch.insert(vch.end(),
            reinterpret_cast<unsigned char *>(&val),
            reinterpret_cast<unsigned char *>(&val) + sizeof(val));
    }

    template<>
    inline void write(std::vector<unsigned char>& vch, std::string value)
    {
        vch.insert(vch.end(),
            reinterpret_cast<unsigned char *>(&value),
            reinterpret_cast<unsigned char *>(&value) + sizeof(value.c_str()));
    }

    template<typename T>
    inline bool read(const std::vector<unsigned char>& vch, T& value, size_t& pos)
    {
        if (vch.size() < pos + sizeof(value)) { return false; }
        value = betoh(*reinterpret_cast<const T*>(&vch[pos]));
        pos += sizeof(value);
        return true;
    }

    template<>
    inline bool read<std::string>(const std::vector<unsigned char>& vch, std::string& value, size_t& pos)
    {
        if (vch.size() < pos + 1) { return false; }

        std::stringstream ss;
        uint8_t c = *reinterpret_cast<const uint8_t*>(&vch[pos]);
        pos += 1;
        while (c != 0) {
            ss << c;
            if (vch.size() < pos + 1) { return false; }
            c = *reinterpret_cast<const uint8_t*>(&vch[pos]);
            pos += 1;
        };
        value = ss.str();
        return true;
    }   

    inline void write(json_spirit::Object& obj, const json_spirit::Pair& value) {
        obj.push_back(value);
    }

    template<typename T>
    inline void write(json_spirit::Object& obj, const T& value, const std::string& label)
    {
        if (std::numeric_limits<T>::is_signed) {
            write(obj, json_spirit::Pair(label, static_cast<int64_t>(value)));
        } else {
            write(obj, json_spirit::Pair(label, static_cast<uint64_t>(value)));
        }
    }

    template<>
    inline void write<std::string>(json_spirit::Object& obj, const std::string& value, const std::string& label)
    {
        write(obj, json_spirit::Pair(label, value));
    }

} // namespace transaction
} // namespace mastercore

#endif // _MASTERCORE_SERIALIZE
