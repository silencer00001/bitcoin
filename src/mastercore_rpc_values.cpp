#include "mastercore_rpc_values.h"

#include "mastercore_sp.h"
#include "mastercore_parse_string.h"

#include "base58.h"
#include "rpcprotocol.h"

#include "json/json_spirit_value.h"


#include <string>


std::string ParseAddress(const json_spirit::Value& value)
{
    CBitcoinAddress address(value.get_str());
    if (!address.IsValid()) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid address");
    }
    return address.ToString();
}

uint32_t ParsePropertyIdUnchecked(const json_spirit::Value& value)
{
    uint64_t propertyId = value.get_uint64();
    if (propertyId < 1 || 4294967295 < propertyId) { // TODO: avoid magic numbers
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid property identifier");
    }
    return static_cast<uint32_t>(propertyId);
}

uint32_t ParsePropertyId(const json_spirit::Value& value, CMPSPInfo::Entry& info)
{
    uint32_t propertyId = ParsePropertyIdUnchecked(value);
    if (!mastercore::_my_sps->getSP(propertyId, info)) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Property identifier does not exist");
    }
    return propertyId;
}

uint32_t ParsePropertyId(const json_spirit::Value& value)
{
    uint32_t propertyId = ParsePropertyIdUnchecked(value);
    if (!mastercore::_my_sps->hasSP(propertyId)) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Property identifier does not exist");
    }
    return propertyId;
}

int64_t ParseAmount(const json_spirit::Value& value, bool fDivisible)
{
    int64_t amount = mastercore::StrToInt64(value.get_str(), fDivisible);
    if (amount <= 0) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid amount");
    }
    if (!isRangeOK(amount)) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Input not in range");
    }
    return amount;
}

uint8_t ParseEcosystem(const json_spirit::Value& value)
{
    uint64_t ecosystem = value.get_uint64();
    if (ecosystem < 1 || 2 < ecosystem) { // TODO: avoid magic numbers
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid ecosystem");
    }
    return static_cast<uint8_t>(ecosystem);
}

uint16_t ParsePropertyType(const json_spirit::Value& value)
{
    uint64_t propertyType = value.get_uint64();
    if (propertyType < 1 || 2 < propertyType) { // TODO: avoid magic numbers
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid type");
    }
    return static_cast<uint16_t>(propertyType);
}

uint32_t ParsePreviousPropertyId(const json_spirit::Value& value)
{
    uint64_t previousId = value.get_uint64();
    if (previousId != 0) { // TODO: avoid magic numbers
        throw JSONRPCError(RPC_TYPE_ERROR, "Property appends/replaces are not yet supported");
    }
    return static_cast<uint32_t>(previousId);
}

