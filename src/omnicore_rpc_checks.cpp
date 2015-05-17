#include "omnicore_rpc_checks.h"

#include "mastercore.h"
#include "mastercore_dex.h"

#include "amount.h"
#include "rpcprotocol.h"

#include <stdint.h>
#include <string>

void RequireSaneReferenceAmount(int64_t amount)
{
    if ((0.01 * COIN) < amount) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid reference amount");
    }
}

void RequireSufficientBalance(const std::string fromAddress, uint32_t propertyId, int64_t amount)
{
    int64_t balance = getMPbalance(fromAddress, propertyId, BALANCE);
    if (balance < amount) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Sender has insufficient balance");
    }

    int64_t balanceUnconfirmed = getUserAvailableMPbalance(fromAddress, propertyId, BALANCE);
    if (balanceUnconfirmed < amount) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Sender has insufficient balance (due to pending transactions)");
    }
}

void RequireNonEmptyPropertyName(const std::string& name)
{
    if (name.empty()) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Property name must not be empty");
    }
}

void RequireOnlyMSC(uint32_t propertyId)
{
    if (propertyId < 1 || 2 < propertyId) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid propertyID for sale - only 1 and 2 are permitted");
    }
}

void RequireActiveCrowdsale(uint32_t propertyId)
{
    if (!mastercore::isCrowdsaleActive(propertyId)) {
        throw JSONRPCError(RPC_TYPE_ERROR, "The specified property does not have a crowdsale active");
    }
}

void RequireTokenAdministrator(const std::string& sender, uint32_t propertyId)
{
    CMPSPInfo::Entry sp;
    mastercore::_my_sps->getSP(propertyId, sp);

    if (sender != sp.issuer) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Sender is not authorized to manage this property");
    }
}

void RequireMatchingDexOffer(const std::string& toAddress, uint32_t propertyId)
{
    if (!mastercore::DEx_offerExists(toAddress, propertyId)) {
        throw JSONRPCError(RPC_TYPE_ERROR, "There is no matching sell offer on the distributed exchange");
    }
}

void RequireNoOtherDexOffer(const std::string& fromAddress, uint32_t propertyId)
{
    if (mastercore::DEx_offerExists(fromAddress, propertyId)) {
        throw JSONRPCError(RPC_TYPE_ERROR, "There is already a sell offer from this address on the distributed exchange, use update instead");
    }
}
