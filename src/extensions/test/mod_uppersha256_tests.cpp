#include "extensions/test/test_utils.h"

#include "extensions/core/modifications/uppersha256.h"

#include "utilstrencodings.h"

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(mod_uppersha256_tests)

static const size_t gMaxSha256ObfuscationTimes = 255;

BOOST_AUTO_TEST_CASE(obfuscate_upper_sha256_test)
{
    std::string strData("0100000000000000010000000002faf080");
    std::string strSeed("1CdighsfdfRcj4ytQSskZgQXbUEamuMUNF");

    std::vector<unsigned char> vchObfuscated;
    vchObfuscated = ObfuscateUpperSha256(ParseHex(strData), strSeed);

    BOOST_CHECK_EQUAL(HexStr(vchObfuscated), "1c9a3de5c2e22bf89b1e41e6fed84fb502");
}

BOOST_AUTO_TEST_CASE(obfuscation_test)
{
    std::string strSeed("1CdighsfdfRcj4ytQSskZgQXbUEamuMUNF");
    std::string pstrObfuscatedHashes[1 + gMaxSha256ObfuscationTimes];
    PrepareObfuscatedHashes(strSeed, pstrObfuscatedHashes);

    BOOST_CHECK_EQUAL(pstrObfuscatedHashes[1],
            "1D9A3DE5C2E22BF89A1E41E6FEDAB54582F8A0C3AE14394A59366293DD130C59");
    BOOST_CHECK_EQUAL(pstrObfuscatedHashes[2],
            "0800ED44F1300FB3A5980ECFA8924FEDB2D5FDBEF8B21BBA6526B4FD5F9C167C");
    BOOST_CHECK_EQUAL(pstrObfuscatedHashes[3],
            "7110A59D22D5AF6A34B7A196DAE7CCC0F27354B34E257832B9955611A9D79B06");
    BOOST_CHECK_EQUAL(pstrObfuscatedHashes[4],
            "AA3F890D32864BEA31EE9BD57D2247D8F8CE07B5ABAED9372F0B8999D28DB963");

    std::vector<std::string> vstrObfuscatedHashes;
    PrepareObfuscatedHashes(strSeed, vstrObfuscatedHashes, gMaxSha256ObfuscationTimes);

    for (size_t n = 0; n < vstrObfuscatedHashes.size(); ++n) {
        BOOST_CHECK_EQUAL(
                vstrObfuscatedHashes[n],
                pstrObfuscatedHashes[n + 1]);
    }
}

BOOST_AUTO_TEST_CASE(obfuscate_back_and_forth_test)
{
    const std::vector<unsigned char> vch(
        ParseHex("0100000000000000010000000002faf0800000000000000000000000000000"));

    const std::string strSeed("1CdighsfdfRcj4ytQSskZgQXbUEamuMUNF");

    std::vector<unsigned char> vch0 = vch;
    std::vector<unsigned char> vch1 = ObfuscateUpperSha256(vch0, strSeed);
    std::vector<unsigned char> vch2 = DeobfuscateUpperSha256(vch1, strSeed);

    BOOST_CHECK_EQUAL(vch0.size(), vch.size());
    BOOST_CHECK_EQUAL(vch0.size(), vch1.size());
    BOOST_CHECK_EQUAL(vch1.size(), vch2.size());
    CHECK_COLLECTIONS(vch0, vch);
    CHECK_COLLECTIONS(vch0, vch2);
}

BOOST_AUTO_TEST_CASE(obfuscate_inplace_test)
{
    const std::vector<unsigned char> vchInput(
        ParseHex("0100000000000000010000000002faf0800000000000000000000000000000"));

    const std::string strSeed("1CdighsfdfRcj4ytQSskZgQXbUEamuMUNF");

    std::vector<unsigned char> vchMutable(vchInput);

    ObfuscateUpperSha256In(vchMutable, strSeed);
    CHECK_COLLECTIONS_NE(vchMutable, vchInput);

    DeobfuscateUpperSha256In(vchMutable, strSeed);
    CHECK_COLLECTIONS(vchMutable, vchInput);
}

BOOST_AUTO_TEST_SUITE_END()
