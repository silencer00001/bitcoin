#include "extensions/rpc/decoding.h"

#include "extensions/core/decoding.h"
#include "extensions/json/conversion.h"
//#include "extensions/primitives/transaction.h"

#include "main.h"
#include "primitives/transaction.h"
#include "rpcprotocol.h"
#include "uint256.h"
#include "utilstrencodings.h"
#include "utiltime.h"

#include "json/json_spirit_value.h"

#include <boost/foreach.hpp>

#include <stdint.h>
#include <stdexcept>
#include <string>
#include <vector>

using namespace json_spirit;

static bool PayloadToJSON(std::vector<unsigned char>& vch, Object& entry)
{
    return true;
}

Value get_payload(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw std::runtime_error(
            "get_payload txid\n"
            "\nIncorrect usage (getpayload_MP is an unofficial function).\n"
    );

    uint256 hash;
    hash.SetHex(params[0].get_str());

    CTransaction tx;
    uint256 hashBlock = 0;
    if (!GetTransaction(hash, tx, hashBlock, true)) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available about transaction");
    }
/*
    CMPTransaction mpObj;
    if (parseTransaction(true, tx, 0, 0, &mpObj) != 0) {
        throw JSONRPCError(RPC_VERIFY_ERROR, "Not an Omni transaction");
    }
*/
    std::string strPayload("00"); // = mpObj.getDecodedPayload();
    std::vector<unsigned char> vchPayload = ParseHex(strPayload);

    Object entry;
    entry.push_back(Pair("txid", hash.GetHex()));
    entry.push_back(Pair("hex", strPayload));

    PayloadToJSON(vchPayload, entry);

    if (hashBlock != 0)
    {
        entry.push_back(Pair("blockhash", hashBlock.GetHex()));
        BlockMap::iterator mi = mapBlockIndex.find(hashBlock);
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

Value decode_payload(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw std::runtime_error(
            "decode_payload hex\n"
            "\nIncorrect usage (decodepacket_MP is an unofficial function).\n"
    );

    std::string strPayload = params[0].get_str();
    std::vector<unsigned char> vchPayload = ParseHex(strPayload);

    if (!IsHex(strPayload)) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Input is not a valid hex");
    }

    Object entry;
    entry.push_back(Pair("hex", strPayload));

    PayloadToJSON(vchPayload, entry);

    return entry;
}
