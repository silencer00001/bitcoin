#include "extensions/rpc/encoding.h"

#include "extensions/core/encoding.h"

#include "base58.h"
#include "pubkey.h"
#include "rpcprotocol.h"
#include "rpcserver.h"
#include "primitives/transaction.h"
#include "script/script.h"
#include "utilstrencodings.h"
#ifdef ENABLE_WALLET
#include "init.h"
#include "wallet.h"
#endif

#include "json/json_spirit_value.h"

#include <boost/foreach.hpp>

#include <stdexcept>
#include <string>
#include <vector>

using namespace json_spirit;

static Object TxOutToJSON(const CTxOut& txout)
{
    Object vout;
    const CScript& script = txout.scriptPubKey;

    vout.push_back(Pair("value", ValueFromAmount(txout.nValue)));
    vout.push_back(Pair("asm", script.ToString()));
    vout.push_back(Pair("hex", HexStr(script.begin(), script.end())));

    return vout;
}

Value null_data(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw std::runtime_error(
            "null_data payload\n"
            "\nIncorrect usage ...\n"
    );

    std::string strPayload = params[0].get_str();
    std::vector<unsigned char> vchPayload(ParseHex(strPayload));

    std::vector<CTxOut> vchTxOutRet;
    EncodeNullData(vchPayload, vchTxOutRet);

    Object result;
    result.push_back(Pair("payload", HexStr(vchPayload)));

    Array vouts;
    BOOST_FOREACH(const CTxOut& txout, vchTxOutRet) {
        Object vout = TxOutToJSON(txout);
        vouts.push_back(vout);
    }

    result.push_back(Pair("packets", vouts));

    return result;
    
}

Value bare_multisig(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 2)
        throw std::runtime_error(
            "bare_multisig payload pubkey\n"
            "\nIncorrect usage ...\n"
    );

    std::string strPayload = params[0].get_str();
    std::string strPubKey = params[1].get_str();    

    CPubKey vchPubKey = CPubKey(ParseHex(strPubKey));
    std::vector<unsigned char> vchPayload(ParseHex(strPayload));

    std::vector<CTxOut> vchTxOutRet;
    EncodeBareMultisig(vchPubKey, vchPayload, vchTxOutRet);

    Object result;
    result.push_back(Pair("redeemer", HexStr(vchPubKey)));
    result.push_back(Pair("payload", HexStr(vchPayload)));

    Array vouts;
    BOOST_FOREACH(const CTxOut& txout, vchTxOutRet) {
        Object vout = TxOutToJSON(txout);
        vouts.push_back(vout);
    }

    result.push_back(Pair("packets", vouts));

    return result;
}

Value obfuscated_multisig(const Array& params, bool fHelp)
{
    if (fHelp || params.size() < 2 || params.size() > 3)
        throw std::runtime_error(
            "obfuscated_multisig address payload ( pubkey )\n"
            "\nIncorrect usage ...\n"
    );

    std::string strAddress = params[0].get_str();
    std::string strPayload = params[1].get_str();    

    CPubKey vchPubKey;
    CBitcoinAddress addrSource(strAddress);
    std::vector<unsigned char> vchPayload(ParseHex(strPayload));

#if ENABLE_WALLET
    if (pwalletMain) {
        CKeyID keyID;
        addrSource.GetKeyID(keyID);
        pwalletMain->GetPubKey(keyID, vchPubKey);
    }
#endif

    if (params.size() > 2) {
        std::string strPubKey = params[2].get_str();
        vchPubKey = CPubKey(ParseHex(strPubKey));
    }

    std::vector<CTxOut> vchTxOutRet;
    EncodeBareMultisigObfuscated(strAddress, vchPubKey, vchPayload, vchTxOutRet);

    Object result;
    result.push_back(Pair("source", strAddress));
    result.push_back(Pair("redeemer", HexStr(vchPubKey)));
    result.push_back(Pair("payload", HexStr(vchPayload)));

    Array vouts;
    BOOST_FOREACH(const CTxOut& txout, vchTxOutRet) {
        Object vout = TxOutToJSON(txout);
        vouts.push_back(vout);
    }

    result.push_back(Pair("packets", vouts));

    return result;
}
