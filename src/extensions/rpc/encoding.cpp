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
            "\nEmbedds a payload in OP_RETURN transaction outputs.\n"
            "\nArguments:\n"
            "1. payload    (string, required) The hex-encoded payload\n"
            "\nResult:\n"
            "{\n"
            "  \"payload\" : \"00000000000000010000000006dac2c0\",            (string) The payload\n"
            "  \"txouts\" : [                                               (array of JSON objects)\n"
            "    {\n"
            "      \"value\" : 0.00000000,                                    (number) The output value\n"
            "      \"asm\" : \"OP_RETURN 00000000000000010000000006dac2c0\",    (string) The script\n"
            "      \"hex\" : \"6a1000000000000000010000000006dac2c0\"           (string) The script as hex\n"
            "    }\n"
            "  ]\n"
            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("null_data", "\"00000000000000010000000006dac2c0\"")
            + HelpExampleRpc("null_data", "\"00000000000000010000000006dac2c0\"")
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

    result.push_back(Pair("txouts", vouts));

    return result;
}

Value bare_multisig(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 2)
        throw std::runtime_error(
            "bare_multisig payload pubkey\n"
            "\nEmbedds a payload in bare-multisig transaction outputs.\n"
            "\nNote: the public key should be used to redeem and spent data outputs.\n"
            "\nArguments:\n"
            "1. payload    (string, required) The hex-encoded payload\n"
            "1. pubkey     (string, required) The public key used for redemption\n"
            "\nResult:\n"
            "{\n"
            "  \"redeemer\" : \"pubkey\",    (string) The public key\n"
            "  \"payload\" : \"hex\",        (string) The payload\n"
            "  \"txouts\" : [              (array of JSON objects)\n"
            "    {\n"
            "      \"value\" : x.xxx,        (number) The output value\n"
            "      \"asm\" : \"asm\",          (string) The script\n"
            "      \"hex\" : \"hex\"           (string) The script as hex\n"
            "    },\n"
            "    ...\n"
            "  ]\n"
            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("bare_multisig",
                "\"00000000000000010000000006dac2c0\" "
                "\"02f3e471222bb57a7d416c82bf81c627bfcd2bdc47f36e763ae69935bba4601ece\"")
            + HelpExampleRpc("bare_multisig",
                "\"00000000000000010000000006dac2c0\","
                "\"02f3e471222bb57a7d416c82bf81c627bfcd2bdc47f36e763ae69935bba4601ece\"")
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

    result.push_back(Pair("txouts", vouts));

    return result;
}

Value obfuscated_multisig(const Array& params, bool fHelp)
{
    if (fHelp || params.size() < 2 || params.size() > 3)
        throw std::runtime_error(
            "obfuscated_multisig address payload ( pubkey )\n"
            "\nEmbedds a payload in obfuscated bare-multisig transaction outputs.\n"
            "\nNote: the public key should be used to redeem and spent data outputs.\n"
            "\nIf no public key is given, and the address is in the wallet, the key is retrieved.\n"
            "\nArguments:\n"
            "1. address    (string, required) An address used as seed for obfuscation\n"
            "2. payload    (string, required) The hex-encoded payload\n"
            "3. pubkey     (string, optional) The public key used for redemption\n"
            "\nResult:\n"
            "{\n"
            "  \"source\" : \"address\",     (string) The address or seed\n"
            "  \"redeemer\" : \"pubkey\",    (string) The public key\n"
            "  \"payload\" : \"hex\",        (string) The payload\n"
            "  \"txouts\" : [              (array of JSON objects)\n"
            "    {\n"
            "      \"value\" : x.xxx,        (number) The output value\n"
            "      \"asm\" : \"asm\",          (string) The script\n"
            "      \"hex\" : \"hex\"           (string) The script as hex\n"
            "    },\n"
            "    ...\n"
            "  ]\n"
            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("obfuscated_multisig",
                "\"1CE8bBr1dYZRMnpmyYsFEoexa1YoPz2mfB\" "
                "\"00000000000000010000000006dac2c0\" "
                "\"02f3e471222bb57a7d416c82bf81c627bfcd2bdc47f36e763ae69935bba4601ece\"")
            + HelpExampleRpc("obfuscated_multisig",
                "\"1CE8bBr1dYZRMnpmyYsFEoexa1YoPz2mfB\","
                "\"00000000000000010000000006dac2c0\","
                "\"02f3e471222bb57a7d416c82bf81c627bfcd2bdc47f36e763ae69935bba4601ece\"")
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

    result.push_back(Pair("txouts", vouts));

    return result;
}
