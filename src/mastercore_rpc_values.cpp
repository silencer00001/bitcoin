#include "mastercore_rpc_values.h"

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