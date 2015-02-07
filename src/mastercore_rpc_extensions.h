#ifndef MASTERCORE_RPC_EXTENSIONS_H
#define MASTERCORE_RPC_EXTENSIONS_H

#include "rpcprotocol.h"
#include "util.h"

#include <limits>
#include <string>

#include <boost/type_traits/is_arithmetic.hpp>

#include "json/json_spirit_value.h"

using json_spirit::Value;
using json_spirit::str_type;

//! Helper for Cast<T>, close copy of ConvertTo<T> (rpcclient.cpp)
static Value Reinterpret(const Value& value) {
    if (value.type() == str_type) {
        // Reinterpret string as unquoted JSON value
        Value value2;
        std::string strJSON = value.get_str();
        if (!read_string(strJSON, value2)) {
            std::string err = strprintf("Error parsing JSON: %s", strJSON);
            throw JSONRPCError(RPC_TYPE_ERROR, err);
        }
        return value2;
    }
    return value;
}

//! Ensures range of casts from uint64 to smaller values
template <typename T, typename NumberType>
T NumericCast(const NumberType& value)
{
    T nMin = std::numeric_limits<T>::min();
    T nMax = std::numeric_limits<T>::max(); 

    if (value < nMin || nMax < value) {
        std::string err = strprintf("Out of range: [%d, %d]", nMin, nMax);
        throw JSONRPCError(RPC_TYPE_ERROR, err);
    }

    return static_cast<T>(value);
}

//! Checked numeric value extraction of Value objects
template<typename T>
T Cast(const Value& value)
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
std::string Cast<std::string>(const Value& value)
{
    return value.get_str();
}

#endif // MASTERCORE_RPC_EXTENSIONS_H
