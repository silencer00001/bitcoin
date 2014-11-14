#include "mastercore_txs/factory.h"
#include "mastercore_txs/transactions.h"

#include "mastercore_tx.h"

#include "main.h"
#include "rpcserver.h"
#include "rpcprotocol.h"
#include "uint256.h"
#include "util.h"

#include <boost/assign/list_of.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include "json/json_spirit_utils.h"
#include "json/json_spirit_value.h"

#include <stdint.h>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

using namespace json_spirit;
using namespace mastercore::transaction;

//! Helper for Cast<T>, close copy of ConvertTo<T> (rpcclient.cpp)
Value Reinterpret(const Value& value, bool fAllowNull = false) {
    if (fAllowNull && value.type() == null_type)
        return value;
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

    std::string strPayload = params[0].get_str();
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

    uint32_t property = Cast<uint32_t>(params[0]);
    int64_t amount = Cast<int64_t>(params[1]);

    CTemplateSimpleSend tx(property, amount);    
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}

Value encode_send_to_owners(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_send_to_owners property amount\n"
    );

    uint32_t property = Cast<uint32_t>(params[0]);
    int64_t amount = Cast<int64_t>(params[1]);

    CTemplateSendToOwners tx(property, amount);    
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}

Value encode_offer_tokens(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 6)
        throw runtime_error(
            "encode_offer_tokens property amount_for_sale amount_desired "
            "block_time_limit commitment_fee subaction\n"
    );

    uint32_t property = Cast<uint32_t>(params[0]);
    int64_t amount_for_sale = Cast<int64_t>(params[1]);
    int64_t amount_desired = Cast<int64_t>(params[2]);
    uint8_t block_time_limit = Cast<uint8_t>(params[3]);
    int64_t commitment_fee = Cast<int64_t>(params[4]);
    uint8_t subaction = Cast<uint8_t>(params[5]);

    CTemplateOfferTokensV1 tx(property, amount_for_sale, amount_desired,
            block_time_limit, commitment_fee, subaction);    
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}

Value encode_trade_tokens(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 5)
        throw runtime_error(
            "encode_trade_tokens property amount_for_sale property_desired "
            "amount_desired subaction\n"
    );

    uint32_t property = Cast<uint32_t>(params[0]);
    int64_t amount_for_sale = Cast<int64_t>(params[1]);
    uint32_t property_desired = Cast<uint32_t>(params[2]);
    int64_t amount_desired = Cast<int64_t>(params[3]);
    uint8_t subaction = Cast<uint8_t>(params[4]);

    CTemplateTradeTokens tx(property, amount_for_sale, property_desired,
            amount_desired, subaction);    
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}

Value encode_accept_offer(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_accept_offer property amount\n"
    );

    uint32_t property = Cast<uint32_t>(params[0]);
    int64_t amount = Cast<int64_t>(params[1]);

    CTemplateAcceptOffer tx(property, amount);    
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}

Value encode_create_property(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 9)
        throw runtime_error(
            "encode_create_property ecosystem property_type previous_property_id "
            "category subcategory name url data amount\n"
    );

    uint8_t ecosystem = Cast<uint8_t>(params[0]);
    uint16_t property_type = Cast<uint16_t>(params[1]);
    uint32_t previous_property_id = Cast<uint32_t>(params[2]);
    std::string category = params[3].get_str();
    std::string subcategory = params[4].get_str();
    std::string name = params[5].get_str();
    std::string url = params[6].get_str();
    std::string data = params[7].get_str();
    int64_t amount = Cast<int64_t>(params[8]);

    CTemplateCreateProperty tx(ecosystem, property_type, previous_property_id,
            category, subcategory, name, url, data, amount);    
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}

Value encode_create_crowdsale(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 13)
        throw runtime_error(
            "encode_create_crowdsale ecosystem property_type previous_property_id "
            "category subcategory name url data property_desired token_per_unit_vested"
            "deadline early_bird_bonus issuer_bonus\n"
    );

    uint8_t ecosystem = Cast<uint8_t>(params[0]);
    uint16_t property_type = Cast<uint16_t>(params[1]);
    uint32_t previous_property_id = Cast<uint32_t>(params[2]);
    std::string category = params[3].get_str();
    std::string subcategory = params[4].get_str();
    std::string name = params[5].get_str();
    std::string url = params[6].get_str();
    std::string data = params[7].get_str();
    uint32_t property_desired = Cast<uint32_t>(params[8]);    
    int64_t token_per_unit_vested = Cast<int64_t>(params[9]);
    uint64_t deadline = Cast<uint64_t>(params[10]);
    uint8_t early_bird_bonus = Cast<uint8_t>(params[11]);
    uint8_t issuer_bonus = Cast<uint8_t>(params[12]);

    CTemplateCreateCrowdsale tx(ecosystem, property_type, previous_property_id, 
            category, subcategory, name, url, data, property_desired, 
            token_per_unit_vested, deadline, early_bird_bonus, issuer_bonus);    
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}

Value encode_close_crowdsale(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "encode_close_crowdsale property\n"
    );

    uint32_t property = Cast<uint32_t>(params[0]);

    CTemplateCloseCrowdsale tx(property);    
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}

Value encode_create_managed_property(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 9)
        throw runtime_error(
            "encode_create_managed_property ecosystem property_type "
            "previous_property_id category subcategory name url data\n"
    );

    uint8_t ecosystem = Cast<uint8_t>(params[0]);
    uint16_t property_type = Cast<uint16_t>(params[1]);
    uint32_t previous_property_id = Cast<uint32_t>(params[2]);
    std::string category = params[3].get_str();
    std::string subcategory = params[4].get_str();
    std::string name = params[5].get_str();
    std::string url = params[6].get_str();
    std::string data = params[7].get_str();

    CTemplateCreateManagedProperty tx(ecosystem, property_type, previous_property_id, 
            category, subcategory, name, url, data);    
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}

Value encode_grant_token(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_grant_token property amount\n"
    );

    uint32_t property = Cast<uint32_t>(params[0]);
    int64_t amount = Cast<int64_t>(params[1]);

    CTemplateGrantToken tx(property, amount);    
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}

Value encode_revoke_token(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_revoke_token property amount\n"
    );

    uint32_t property = Cast<uint32_t>(params[0]);
    int64_t amount = Cast<int64_t>(params[1]);

    CTemplateRevokeToken tx(property, amount);    
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}
