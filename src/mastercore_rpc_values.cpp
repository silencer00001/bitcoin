#include "mastercore_rpc_values.h"

#include "mastercore_sp.h"
#include "mastercore_parse_string.h"

#include "base58.h"
#include "rpcprotocol.h"

#include "json/json_spirit_value.h"

#include <string>

using mastercore::StrToInt64;

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
    int64_t propertyId = value.get_int64();
    if (propertyId < 1 || 4294967295 < propertyId) {
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
    if (0 < amount) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid amount");
    }
    return amount;
}

int64_t ParseAmount(const json_spirit::Value& value, int propertyType)
{
    bool fDivisible = (propertyType == 2);  // 1 = indivisible, 2 = divisible
    return ParseAmount(value, fDivisible);
}

uint8_t ParseEcosystem(const json_spirit::Value& value)
{
    uint64_t ecosystem = value.get_uint64();
    if (ecosystem < 1 || 2 < ecosystem) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid ecosystem");
    }
    return static_cast<uint8_t>(ecosystem);
}

uint16_t ParsePropertyType(const json_spirit::Value& value)
{
    int64_t propertyType = value.get_int64();
    if (propertyType < 1 || 2 < propertyType) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid type");
    }
    return static_cast<uint16_t>(propertyType);
}

uint32_t ParsePreviousPropertyId(const json_spirit::Value& value)
{
    int64_t previousId = value.get_int64();
    if (previousId != 0) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Property appends/replaces are not yet supported");
    }
    return static_cast<uint32_t>(previousId);
}

std::string ParseText(const json_spirit::Value& value)
{
    std::string text = value.get_str();
    if (text.size() > 255) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Text must not be longer than 255 characters");
    }
    return text;
}

uint8_t ParsePaymentTimeframe(const json_spirit::Value& value)
{
    int64_t blocks = value.get_int64();
    if (blocks < 1 || 255 < blocks) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Payment window invalid");
    }
    return static_cast<uint8_t>(blocks);
}

int64_t ParseDeadline(const json_spirit::Value& value)
{
    int64_t deadline = value.get_int64();
    if (deadline < 0) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Deadline must be positive");
    }
    return deadline;
}

uint8_t ParseDexAction(const json_spirit::Value& value)
{
    int64_t action = value.get_int64();
    if (action <= 0 || 3 < action) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid action (1, 2, 3 only)");
    }
    return static_cast<uint8_t>(action);
}

uint8_t ParseMetaDexAction(const json_spirit::Value& value)
{
    int64_t action = value.get_int64();
    if (action <= 0 || 4 < action) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid action (1, 2, 3, 4 only)");
    }
    return static_cast<uint8_t>(action);
}

int64_t ParseCommitmentFee(const json_spirit::Value& value)
{
    int64_t minfee = StrToInt64(value.get_str(), true); // BTC so always divisible
    if (minfee < 0) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Mininmum accept mining fee invalid");
    }
    return minfee;
}

uint8_t ParseEarlyBirdBonus(const json_spirit::Value& value)
{
    int64_t percentage = value.get_int64();
    if (percentage < 0 || 255 < percentage) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Early bird bonus must be in the range 0-255");
    }
    return static_cast<uint8_t>(percentage);
}

uint8_t ParseIssuerBonus(const json_spirit::Value& value)
{
    int64_t percentage = value.get_int64();
    if (percentage < 0 || 255 < percentage) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Bonus percentage for issuer be in the range 0-255");
    }
    return static_cast<uint8_t>(percentage);
}

