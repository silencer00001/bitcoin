#include "mastercore_rpc_extensions.h"

#include "mastercore_txs/factory.h"
#include "mastercore_txs/transactions.h"
#include "mastercore_tx.h"
#include "mastercore_types.h"

#include "main.h"
#include "rpcserver.h"
#include "rpcprotocol.h"
#include "uint256.h"
#include "util.h"

#include "json/json_spirit_utils.h"
#include "json/json_spirit_value.h"

#include <stdint.h>
#include <map>
#include <string>
#include <vector>

using namespace json_spirit;
using namespace mastercore::transaction;
using namespace mastercore::types;

static bool PayloadToJSON(std::vector<unsigned char>& vch, Object& entry)
{
    CTemplateBase* tmplTx = tryDeserializeTransaction(vch);

    if (NULL == tmplTx) {
        return false;
    }

    tmplTx->Serialize(entry);
    delete tmplTx;

    return true;
}

static std::string EncodeAsHex(const CTemplateBase& tx)
{
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}

Value getpayload_MP(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getpayload_MP txid\n"
            "\nIncorrect usage (getpayload_MP is an unofficial function).\n"
    );

    uint256 hash;
    hash.SetHex(params[0].get_str());

    CTransaction tx;
    uint256 hashBlock = 0;
    if (!GetTransaction(hash, tx, hashBlock, true)) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available about transaction");
    }

    CMPTransaction mpObj;
    if (parseTransaction(true, tx, 0, 0, &mpObj) != 0) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Not an Omni transaction");
    }

    std::string strPayload = mpObj.getDecodedPayload();
    std::vector<unsigned char> vchPayload = ParseHex(strPayload);

    Object entry;
    entry.push_back(Pair("txid", hash.GetHex()));
    entry.push_back(Pair("hex", strPayload));

    PayloadToJSON(vchPayload, entry);

    if (hashBlock != 0)
    {
        entry.push_back(Pair("blockhash", hashBlock.GetHex()));
        std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hashBlock);
        if (mi != mapBlockIndex.end() && (*mi).second)
        {
            CBlockIndex* pindex = (*mi).second;
            if (chainActive.Contains(pindex))
            {
                entry.push_back(Pair("confirmations", 1 + chainActive.Height() - pindex->nHeight));
                entry.push_back(Pair("time", (int64_t)pindex->nTime));
                entry.push_back(Pair("blocktime", (int64_t)pindex->nTime));
            }
            else
                entry.push_back(Pair("confirmations", 0));
        }
    }

    return entry;
}

Value decodepacket_MP(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "decodepacket_MP hex\n"
            "\nIncorrect usage (decodepacket_MP is an unofficial function).\n"
    );

    std::string strPayload = Cast<std::string>(params[0]);
    std::vector<unsigned char> vchPayload = ParseHex(strPayload);

    if (!IsHex(strPayload)) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Input is not a valid hex");
    }

    Object entry;
    entry.push_back(Pair("hex", strPayload));

    PayloadToJSON(vchPayload, entry);

    return entry;
}

Value encode_simple_send(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_simple_send property amount\n"
    );

    CTemplateSimpleSend tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]));

    return EncodeAsHex(tx);
}

Value encode_send_to_owners(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_send_to_owners property amount\n"
    );

    CTemplateSendToOwners tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]));

    return EncodeAsHex(tx);
}

Value encode_offer_tokens(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 6)
        throw runtime_error(
            "encode_offer_tokens property amount_for_sale amount_desired "
            "block_time_limit commitment_fee subaction\n"
    );

    CTemplateOfferTokensV1 tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]),
            Cast<TokenAmountType>(params[2]),
            Cast<BlockIntervalType>(params[3]),
            Cast<TokenAmountType>(params[4]),
            Cast<DexActionType>(params[5]));

    return EncodeAsHex(tx);
}

Value encode_trade_tokens(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 5)
        throw runtime_error(
            "encode_trade_tokens property amount_for_sale property_desired "
            "amount_desired subaction\n"
    );

    CTemplateTradeTokens tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]),
            Cast<PropertyIdentifierType>(params[2]),
            Cast<TokenAmountType>(params[3]),
            Cast<MetaDexActionType>(params[4]));

    return EncodeAsHex(tx);
}

Value encode_accept_offer(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_accept_offer property amount\n"
    );

    CTemplateAcceptOffer tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]));

    return EncodeAsHex(tx);
}

Value encode_create_property(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 9)
        throw runtime_error(
            "encode_create_property ecosystem property_type previous_property_id "
            "category subcategory name url data amount\n"
    );

    CTemplateCreateProperty tx(
            Cast<EcosystemType>(params[0]),
            Cast<PropertyType>(params[1]),
            Cast<PropertyIdentifierType>(params[2]),
            Cast<StringType>(params[3]),
            Cast<StringType>(params[4]),
            Cast<StringType>(params[5]),
            Cast<StringType>(params[6]),
            Cast<StringType>(params[7]),
            Cast<TokenAmountType>(params[8]));

    return EncodeAsHex(tx);
}

Value encode_create_crowdsale(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 13)
        throw runtime_error(
            "encode_create_crowdsale ecosystem property_type previous_property_id "
            "category subcategory name url data property_desired token_per_unit_vested"
            "deadline early_bird_bonus issuer_bonus\n"
    );

    CTemplateCreateCrowdsale tx(
            Cast<EcosystemType>(params[0]),
            Cast<PropertyType>(params[1]),
            Cast<PropertyIdentifierType>(params[2]),
            Cast<StringType>(params[3]),
            Cast<StringType>(params[4]),
            Cast<StringType>(params[5]),
            Cast<StringType>(params[6]),
            Cast<StringType>(params[7]),
            Cast<PropertyIdentifierType>(params[8]),            
            Cast<TokenAmountType>(params[9]),
            Cast<TimestampType>(params[10]),
            Cast<PercentageType>(params[11]),
            Cast<PercentageType>(params[12]));

    return EncodeAsHex(tx);
}

Value encode_close_crowdsale(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "encode_close_crowdsale property\n"
    );

    CTemplateCloseCrowdsale tx(
            Cast<PropertyIdentifierType>(params[0]));

    return EncodeAsHex(tx);
}

Value encode_create_managed_property(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 9)
        throw runtime_error(
            "encode_create_managed_property ecosystem property_type "
            "previous_property_id category subcategory name url data\n"
    );

    CTemplateCreateManagedProperty tx(
            Cast<EcosystemType>(params[0]),
            Cast<PropertyType>(params[1]),
            Cast<PropertyIdentifierType>(params[2]),
            Cast<StringType>(params[3]),
            Cast<StringType>(params[4]),
            Cast<StringType>(params[5]),
            Cast<StringType>(params[6]),
            Cast<StringType>(params[7]));

    return EncodeAsHex(tx);
}

Value encode_grant_token(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_grant_token property amount\n"
    );

    CTemplateGrantToken tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]));

    return EncodeAsHex(tx);
}

Value encode_revoke_token(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_revoke_token property amount\n"
    );

    CTemplateRevokeToken tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]));

    return EncodeAsHex(tx);
}

Value encode_change_issuer(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "encode_change_issuer property\n"
    );

    CTemplateChangeIssuer tx(
            Cast<PropertyIdentifierType>(params[0]));

    return EncodeAsHex(tx);
}
