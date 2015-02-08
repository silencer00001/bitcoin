#include "mastercore_tx.h"
#include "mastercore_txs/factory.h"
#include "mastercore_txs/transactions.h"

#include "main.h"
#include "rpcprotocol.h"
#include "rpcserver.h"
#include "uint256.h"
#include "util.h"

#include "json/json_spirit_value.h"

#include <stdint.h>
#include <map>
#include <string>
#include <stdexcept>
#include <vector>

using std::runtime_error;
using namespace json_spirit;
using namespace mastercore::transaction;

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
