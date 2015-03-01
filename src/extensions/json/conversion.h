#ifndef EXTENSIONS_JSON_CONVERSION_H
#define EXTENSIONS_JSON_CONVERSION_H

#include "rpcprotocol.h"
#include "tinyformat.h"

#include "json/json_spirit_value.h"

#include <limits>
#include <string>

#include <boost/type_traits/is_arithmetic.hpp>

//! Reinterprets string Value objects as unquoted JSON value
static json_spirit::Value Reinterpret(const json_spirit::Value& value)
{
    if (value.type() == json_spirit::str_type) {
        json_spirit::Value value2;
        std::string strJSON = value.get_str();
        if (!json_spirit::read_string(strJSON, value2)) {
            std::string err = strprintf("Error parsing JSON: %s", strJSON);
            throw JSONRPCError(RPC_TYPE_ERROR, err);
        }
        return value2;
    }
    return value;
}

//! Converts numeric values, constrained by target range
template <typename T, typename NumberType>
static T NumericCast(const NumberType& value)
{
    T nMin = std::numeric_limits<T>::min();
    T nMax = std::numeric_limits<T>::max();

    if (value < nMin || nMax < value) {
        std::string err = strprintf("Out of range: [%d, %d]", nMin, nMax);
        throw JSONRPCError(RPC_TYPE_ERROR, err);
    }

    return static_cast<T>(value);
}

//! Converts Value objects to native data types
template<typename T>
T Cast(const json_spirit::Value& value)
{
    if (boost::is_arithmetic<T>::value && std::numeric_limits<T>::is_signed) {
        return NumericCast<T>(Reinterpret(value).get_int64());
    }

    if (boost::is_arithmetic<T>::value) {
        return NumericCast<T>(Reinterpret(value).get_uint64());
    }

    throw JSONRPCError(RPC_TYPE_ERROR, "Unsupported value type");
}

template<>
std::string Cast<std::string>(const json_spirit::Value& value)
{
    return value.get_str();
}

template<>
bool Cast<bool>(const json_spirit::Value& value)
{
    return Reinterpret(value).get_bool();
}

template<>
double Cast<double>(const json_spirit::Value& value)
{
    return Reinterpret(value).get_real();
}

template<>
json_spirit::Array Cast<json_spirit::Array>(const json_spirit::Value& value)
{
    return Reinterpret(value).get_array();
}

template<>
json_spirit::Object Cast<json_spirit::Object>(const json_spirit::Value& value)
{
    return Reinterpret(value).get_obj();
}

#endif // EXTENSIONS_JSON_CONVERSION_H
