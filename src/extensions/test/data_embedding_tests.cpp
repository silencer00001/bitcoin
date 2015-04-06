#include "extensions/test/test_utils.h"

#include "extensions/core/decoding.h"
#include "extensions/core/encoding.h"

#include "primitives/transaction.h"
#include "pubkey.h"
#include "script/script.h"
#include "script/standard.h"
#include "utilstrencodings.h"

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(data_embedding_tests)

BOOST_AUTO_TEST_CASE(encode_bare_multisig_test)
{
    const CPubKey pubKey(ParseHex(
        "0347d08029b5cbc934f6079b650c50718eab5a56d51cf6b742ec9f865a41fcfca3"));
    const std::vector<unsigned char> vchPayload(ParseHex(
        "00000000000000010000000002faf080"));

    std::vector<CTxOut> vTxOuts;
    BOOST_CHECK(EncodeBareMultisig(pubKey, vchPayload, vTxOuts));
    BOOST_CHECK_EQUAL(vTxOuts.size(), 1);
}

BOOST_AUTO_TEST_CASE(encode_decode_bare_multisig_test)
{
    const std::vector<unsigned char> vchPubKey = ParseHex(
        "02f3e471222bb57a7d416c82bf81c627bfcd2bdc47f36e763ae69935bba4601ece");

    const std::vector<unsigned char> vchPayloadIn = ParseHex(
        "00000000000000010000000006dac2c0000000000000000000000000000000");

    std::vector<CTxOut> vTxOuts;
    BOOST_CHECK(EncodeBareMultisig(vchPubKey, vchPayloadIn, vTxOuts));

    std::vector<unsigned char> vchPayloadOut;
    BOOST_CHECK(DecodeBareMultisig(vTxOuts, vchPayloadOut));
    BOOST_CHECK(IsEqual(vchPayloadIn, vchPayloadOut));
}

BOOST_AUTO_TEST_CASE(encode_decode_obfuscated_multisig_test)
{
    // Transaction hash (mainnet):
    // 701d4a968af0cc7d11a66c64f023482dbef0cde06f466e39cef75ee7b86627cf
    const std::string strSeed(
        "1f2dj45pxYb8BCW5sSbCgJ5YvXBfSapeX");

    const std::vector<unsigned char> vchPubKey = ParseHex(
        "044ab250ef237ef1e96300f530fc9fded23e0aa93221f69f7f767e07872cf35e60"
          "77f0a000910705555f0c7b43597b0435b67b93e6888484aac85d3a630715699f");

    const std::vector<unsigned char> vchPayloadIn = ParseHex(
        "00000032010002000000004e6f6e70726f666974204f7267616e697a6174"
        "696f6e00476c6f62616c204f7267616e697a6174696f6e00484f50452047"
        "6f6c6420436f696e00687474703a2f2f7777772e686f7065676f6c64636f"
        "696e2e6f72672f00484f5045203d204865616c7468202013204f70706f72"
        "74756e69747920201320507572706f736520201320456475636174696f6e"
        "2e204e6f6e2d50726f6669742043727970746f2043757272656e63792062"
        "61636b656420627920476f6c642064656469636174656420746f20616464"
        "72657373696e6720756e6976657273616c20736f6369616c2c20656e7669"
        "726f6e6d656e74616c2c206865616c74682c20666f6f642c2065636f6e6f"
        "6d696320262073696d696c61722070726f626c656d73207468617420696d"
        "7061637420746865206c69766573206f662070656f706c6520776f726c64"
        "776964652e000186cc6acd4b000000000000000000000000000000000000");

    std::vector<CTxOut> vTxOuts;
    BOOST_CHECK(EncodeBareMultisigObfuscated(strSeed, vchPubKey, vchPayloadIn, vTxOuts));

    std::vector<unsigned char> vchPayloadOut;
    BOOST_CHECK(DecodeBareMultisigObfuscated(strSeed, vTxOuts, vchPayloadOut));
    CHECK_COLLECTIONS(vchPayloadIn, vchPayloadOut);
}

BOOST_AUTO_TEST_CASE(decode_obfuscated_multisig_mainsafe_test)
{
    // Transaction hash (mainnet):
    // 86f214055a7f4f5057922fd1647e00ef31ab0a3ff15217f8b90e295f051873a7
    const std::string strSeed("1ARjWDkZ7kT9fwjPrjcQyvbXDkEySzKHwu");

    const std::vector<unsigned char> vchPubKeyA1 = ParseHex(
        "02619c30f643a4679ec2f690f3d6564df7df2ae23ae4a55393ae0bef22db9dbcaf");
    const std::vector<unsigned char> vchPubKeyA2 = ParseHex(
        "026766a63686d2cc5d82c929d339b7975010872aa6bf76f6fac69f28f8e293a914");
    const std::vector<unsigned char> vchPubKeyA3 = ParseHex(
        "02959b8e2f2e4fb67952cda291b467a1781641c94c37feaa0733a12782977da23b");

    const std::vector<unsigned char> vchPubKeyB1 = ParseHex(
        "02619c30f643a4679ec2f690f3d6564df7df2ae23ae4a55393ae0bef22db9dbcaf");
    const std::vector<unsigned char> vchPubKeyB2 = ParseHex(
        "0261a017029ec4688ec9bf33c44ad2e595f83aaf3ed4f3032d1955715f5ffaf6b8");
    const std::vector<unsigned char> vchPubKeyB3 = ParseHex(
        "02dc1a0afc933d703557d9f5e86423a5cec9fee4bfa850b3d02ceae72117178802");

    std::vector<CPubKey> vPubKeysA;
    vPubKeysA.push_back(CPubKey(vchPubKeyA1.begin(), vchPubKeyA1.end()));
    vPubKeysA.push_back(CPubKey(vchPubKeyA2.begin(), vchPubKeyA2.end()));
    vPubKeysA.push_back(CPubKey(vchPubKeyA3.begin(), vchPubKeyA3.end()));

    std::vector<CPubKey> vPubKeysB;
    vPubKeysB.push_back(CPubKey(vchPubKeyB1.begin(), vchPubKeyB1.end()));
    vPubKeysB.push_back(CPubKey(vchPubKeyB2.begin(), vchPubKeyB2.end()));
    vPubKeysB.push_back(CPubKey(vchPubKeyB3.begin(), vchPubKeyB3.end()));

    std::vector<CTxOut> vTxOuts;
    vTxOuts.push_back(CTxOut(0.000165, GetScriptForMultisig(1, vPubKeysA)));
    vTxOuts.push_back(CTxOut(0.000165, GetScriptForMultisig(1, vPubKeysB)));

    std::vector<unsigned char> vchPayload;
    BOOST_CHECK(DecodeBareMultisigObfuscated(strSeed, vTxOuts, vchPayload));
    CHECK_COLLECTIONS(vchPayload, ParseHex(
        // Transaction version: 0
        "0000"
        // Transaction type: Create Crowdsale (51)
        "0033"
        // Eco system: Main (1)
        "01"
        // Property type: Indivisible tokens (1)
        "0001"
        // Previous property identifier: None (0)
        "00000000"
        // Category: "Crowdsale"
        "43726f776473616c6500"
        // Sub category: "MaidSafe"
        "4d6169645361666500"
        // Property name: "MaidSafeCoin"
        "4d61696453616665436f696e00"
        // URL: "www.buysafecoins.com"
        "7777772e62757973616665636f696e732e636f6d00"
        // Information: "SAFE Network Crowdsale (MSAFE)"
        "53414645204e6574776f726b2043726f776473616c6520284d534146452900"
        // Desired property: Mastercoin (SP #1)
        "00000001"
        // Amount per unit invested: 3400
        "0000000000000d48"
        // Deadline: Thu, 22 May 2014 09:00:00 UTC (1400749200)
        "00000000537dbc90"
        // Early bird bonus: 10 % per week
        "0a"
        // Percentage for issuer: 0 %
        "00"
        // Payload padding
        "000000"));
}

BOOST_AUTO_TEST_CASE(decode_obfuscated_multisig_tetherus_test)
{
    // Transaction hash (mainnet):
    // 5ed3694e8a4fa8d3ec5c75eb6789492c69e65511522b220e94ab51da2b6dd53f
    const std::string strSeed("3MbYQMMmSkC3AgWkj9FMo5LsPTW1zBTwXL");

    const std::vector<unsigned char> vchPubKeyA1 = ParseHex(
        "04ad90e5b6bc86b3ec7fac2c5fbda7423fc8ef0d58df594c773fa05e2c281b2bfe"
          "877677c668bd13603944e34f4818ee03cadd81a88542b8b4d5431264180e2c28");
    const std::vector<unsigned char> vchPubKeyA2 = ParseHex(
        "02f88f01791557f6d57e6b7ddf86d2de2117e6cc4ba325a4e309d4a1a55015d785");
    const std::vector<unsigned char> vchPubKeyA3 = ParseHex(
        "02a94f47f4c3b8c36876399f19ecd61cf452248330fa5da9a1947d6dc7a189a134");

    const std::vector<unsigned char> vchPubKeyB1 = ParseHex(
        "04ad90e5b6bc86b3ec7fac2c5fbda7423fc8ef0d58df594c773fa05e2c281b2bfe"
          "877677c668bd13603944e34f4818ee03cadd81a88542b8b4d5431264180e2c28");
    const std::vector<unsigned char> vchPubKeyB2 = ParseHex(
        "026d7e7235fc2c6769e351196c9ccdc4c804184b5bb9b210f27d3f0a613654fe38");
    const std::vector<unsigned char> vchPubKeyB3 = ParseHex(
        "028991cff7cc6d93c266615d2a9223cef4d7b11c05c16b0cec12a90ee7b39cf85d");

    const std::vector<unsigned char> vchPubKeyC1 = ParseHex(
        "04ad90e5b6bc86b3ec7fac2c5fbda7423fc8ef0d58df594c773fa05e2c281b2bfe"
          "877677c668bd13603944e34f4818ee03cadd81a88542b8b4d5431264180e2c28");
    const std::vector<unsigned char> vchPubKeyC2 = ParseHex(
        "0229b3e0919adc41a316aad4f41444d9bf3a9b639550f2aa735676ffff25ba3898");
    const std::vector<unsigned char> vchPubKeyC3 = ParseHex(
        "02f15446771c5c585dd25d8d62df5195b77799aa8eac2f2196c54b73ca05f72f27");

    std::vector<CPubKey> vPubKeysA;
    vPubKeysA.push_back(CPubKey(vchPubKeyA1.begin(), vchPubKeyA1.end()));
    vPubKeysA.push_back(CPubKey(vchPubKeyA2.begin(), vchPubKeyA2.end()));
    vPubKeysA.push_back(CPubKey(vchPubKeyA3.begin(), vchPubKeyA3.end()));

    std::vector<CPubKey> vPubKeysB;
    vPubKeysB.push_back(CPubKey(vchPubKeyB1.begin(), vchPubKeyB1.end()));
    vPubKeysB.push_back(CPubKey(vchPubKeyB2.begin(), vchPubKeyB2.end()));
    vPubKeysB.push_back(CPubKey(vchPubKeyB3.begin(), vchPubKeyB3.end()));

    std::vector<CPubKey> vPubKeysC;
    vPubKeysC.push_back(CPubKey(vchPubKeyC1.begin(), vchPubKeyC1.end()));
    vPubKeysC.push_back(CPubKey(vchPubKeyC2.begin(), vchPubKeyC2.end()));
    vPubKeysC.push_back(CPubKey(vchPubKeyC3.begin(), vchPubKeyC3.end()));

    std::vector<CTxOut> vTxOuts;
    vTxOuts.push_back(CTxOut(0.000165, GetScriptForMultisig(1, vPubKeysA)));
    vTxOuts.push_back(CTxOut(0.000165, GetScriptForMultisig(1, vPubKeysB)));
    vTxOuts.push_back(CTxOut(0.000165, GetScriptForMultisig(1, vPubKeysC)));

    std::vector<unsigned char> vchPayload;
    BOOST_CHECK(DecodeBareMultisigObfuscated(strSeed, vTxOuts, vchPayload));
    CHECK_COLLECTIONS(vchPayload, ParseHex(
        "000000360100020000000046696e616e6369616c20616e6420696e737572"
        "616e63652061637469766974696573004163746976697469657320617578"
        "696c6961727920746f2066696e616e6369616c207365727669636520616e"
        "6420696e737572616e636520616374697669746965730054657468657255"
        "530068747470733a2f2f7465746865722e746f00546865206e6578742070"
        "6172616469676d206f66206d6f6e65792e00000000000000000000000000"));
}

BOOST_AUTO_TEST_SUITE_END()
