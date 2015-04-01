#include "extensions/test/test_utils.h"

#include "extensions/bitcoin/script.h"

#include "base58.h"
#include "pubkey.h"
#include "script/script.h"
#include "script/standard.h"
#include "utilstrencodings.h"

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(script_extraction_tests)

BOOST_AUTO_TEST_CASE(extract_pubkey_test)
{
    std::vector<unsigned char> vchPayload = ParseHex(
        "0347d08029b5cbc934f6079b650c50718eab5a56d51cf6b742ec9f865a41fcfca3");

    CPubKey pubKey(vchPayload.begin(), vchPayload.end());

    // pay-to-pubkey script
    CScript script;
    script << ToByteVector(pubKey) << OP_CHECKSIG;

    // confirm script type
    txnouttype outtype;
    BOOST_CHECK(GetOutputType(script, outtype));
    BOOST_CHECK_EQUAL(outtype, TX_PUBKEY);

    // confirm extracted data
    std::vector<std::vector<unsigned char> > vvchSolutions;
    BOOST_CHECK(GetScriptPushes(script, vvchSolutions));
    BOOST_CHECK_EQUAL(vvchSolutions.size(), 1);
    BOOST_CHECK(IsEqual(vvchSolutions[0], vchPayload));
}

BOOST_AUTO_TEST_CASE(extract_pubkeyhash_test)
{
    std::vector<unsigned char> vchPayload = ParseHex(
        "0347d08029b5cbc934f6079b650c50718eab5a56d51cf6b742ec9f865a41fcfca3");

    CPubKey pubKey(vchPayload.begin(), vchPayload.end());
    CKeyID keyId = pubKey.GetID();

    // pay-to-pubkey-hash script
    CScript script;
    script << OP_DUP << OP_HASH160 << ToByteVector(keyId) << OP_EQUALVERIFY << OP_CHECKSIG;

    // confirm script type
    txnouttype outtype;
    BOOST_CHECK(GetOutputType(script, outtype));
    BOOST_CHECK_EQUAL(outtype, TX_PUBKEYHASH);

    // confirm extracted data
    std::vector<std::vector<unsigned char> > vvchSolutions;
    BOOST_CHECK(GetScriptPushes(script, vvchSolutions));
    BOOST_CHECK_EQUAL(vvchSolutions.size(), 1);
    BOOST_CHECK(IsEqual(vvchSolutions[0], ToByteVector(keyId)));
}

BOOST_AUTO_TEST_CASE(extract_multisig_test)
{
    std::vector<unsigned char> vchPayload1 = ParseHex(
        "03a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd");
    std::vector<unsigned char> vchPayload2 = ParseHex(
        "0276f798620d7d0930711ab68688fc67ee2f5bbe0c1481506b08bd65e6053c16ca");
    std::vector<unsigned char> vchPayload3 = ParseHex(
        "02bf12b315172dc1b261d62dd146868ef9c9e2e108fa347f347f66bc048e9b15e4");

    CPubKey pubKey1(vchPayload1.begin(), vchPayload1.end());
    CPubKey pubKey2(vchPayload2.begin(), vchPayload2.end());
    CPubKey pubKey3(vchPayload3.begin(), vchPayload3.end());

    // 1-of-3 multisig script
    CScript script;
    script << CScript::EncodeOP_N(1);
    script << ToByteVector(pubKey1) << ToByteVector(pubKey2) << ToByteVector(pubKey3);
    script << CScript::EncodeOP_N(3);
    script << OP_CHECKMULTISIG;

    // confirm script type
    txnouttype outtype;
    BOOST_CHECK(GetOutputType(script, outtype));
    BOOST_CHECK_EQUAL(outtype, TX_MULTISIG);

    // confirm extracted data
    std::vector<std::vector<unsigned char> > vvchSolutions;
    BOOST_CHECK(GetScriptPushes(script, vvchSolutions));
    BOOST_CHECK_EQUAL(vvchSolutions.size(), 3);
    BOOST_CHECK(IsEqual(vvchSolutions[0], vchPayload1));
    BOOST_CHECK(IsEqual(vvchSolutions[1], vchPayload2));
    BOOST_CHECK(IsEqual(vvchSolutions[2], vchPayload3));
}

BOOST_AUTO_TEST_CASE(extract_scripthash_test)
{
    std::vector<unsigned char> vchInnerScript = ParseHex(
        "6fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d6190000000000");

    // a transaction puzzle
    CScript scriptInner;
    scriptInner << OP_HASH256 << vchInnerScript << OP_EQUAL;

    // the actual hash
    CScriptID innerId(scriptInner);

    // pay-to-script-hash
    CScript script;
    script << OP_HASH160 << ToByteVector(innerId) << OP_EQUAL;

    // confirm script type
    txnouttype outtype;
    BOOST_CHECK(GetOutputType(script, outtype));
    BOOST_CHECK_EQUAL(outtype, TX_SCRIPTHASH);

    // confirm extracted data
    std::vector<std::vector<unsigned char> > vvchSolutions;
    BOOST_CHECK(GetScriptPushes(script, vvchSolutions));
    BOOST_CHECK_EQUAL(vvchSolutions.size(), 1);
    BOOST_CHECK(IsEqual(vvchSolutions[0], ToByteVector(innerId)));
}

BOOST_AUTO_TEST_CASE(extract_nulldata_test)
{
    std::vector<unsigned char> vchPayload = ParseHex(
        "657874726163745f6e756c6c646174615f74657374");

    // null data script
    CScript script;
    script << OP_RETURN << vchPayload;

    // confirm script type
    txnouttype outtype;
    BOOST_CHECK(GetOutputType(script, outtype));
    BOOST_CHECK_EQUAL(outtype, TX_NULL_DATA);

    // confirm extracted data
    std::vector<std::vector<unsigned char> > vvchSolutions;
    BOOST_CHECK(GetScriptPushes(script, vvchSolutions));
    BOOST_CHECK_EQUAL(vvchSolutions.size(), 1);
    BOOST_CHECK(IsEqual(vvchSolutions[0], vchPayload));
}

BOOST_AUTO_TEST_CASE(extract_anypush_test)
{
    std::vector<std::vector<unsigned char> > vvchPayloads;
    vvchPayloads.push_back(ParseHex("111111"));
    vvchPayloads.push_back(ParseHex("222222"));
    vvchPayloads.push_back(ParseHex("333333"));
    vvchPayloads.push_back(ParseHex("444444"));
    vvchPayloads.push_back(ParseHex("555555"));

    CScript script;
    script << vvchPayloads[0] << OP_DROP;
    script << vvchPayloads[1] << OP_DROP;
    script << vvchPayloads[2] << OP_DROP;
    script << vvchPayloads[3] << OP_DROP;
    script << vvchPayloads[4];

    // confirm extracted data
    std::vector<std::vector<unsigned char> > vvchSolutions;
    BOOST_CHECK(GetScriptPushes(script, vvchSolutions));
    BOOST_CHECK_EQUAL(vvchSolutions.size(), vvchPayloads.size());
    for (size_t n = 0; n < vvchSolutions.size(); ++n) {
        BOOST_CHECK(IsEqual(vvchSolutions[n], vvchPayloads[n]));
    }
}

BOOST_AUTO_TEST_SUITE_END()
