#include "extensions/test/test_utils.h"

#include "extensions/core/modifications/pubkeys.h"

#include "pubkey.h"
#include "utilstrencodings.h"

#include <cstdlib>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(mod_pubkeys_tests)

BOOST_AUTO_TEST_CASE(create_valid_pubkey_test)
{
    CPubKey pubKey;
    BOOST_CHECK(!pubKey.IsValid());

    // Created public keys are valid
    pubKey = CreatePubKey(ParseHex("00"));
    BOOST_CHECK(pubKey.IsValid());
    BOOST_CHECK(pubKey.IsFullyValid());
}

BOOST_AUTO_TEST_CASE(pubkey_ecdsa_modfication_test)
{
    // Not fully valid public key
    const std::vector<unsigned char> vchPubKeyInvalid(ParseHex(
        "02777ac9576cb08fb869efd4be2ca094c55808054e2220285f3c754d59361b3007"));

    // Modified public key, which represents a valid ECDSA coordinate
    const std::vector<unsigned char> vchPubKeyExpected(ParseHex(
        "02777ac9576cb08fb869efd4be2ca094c55808054e2220285f3c754d59361b300c"));

    CPubKey pubKey(vchPubKeyInvalid);
    CPubKey pubKeyExpected(vchPubKeyExpected);

    BOOST_CHECK(!pubKey.IsFullyValid());
    BOOST_CHECK(ModifyEcdsaPoint(pubKey));
    BOOST_CHECK(pubKey.IsFullyValid());
    BOOST_CHECK(IsEqual(pubKey, pubKeyExpected));
}

static void checkPubKeyPayload(const std::string& strIn, const std::string& strExpected)
{
    // Convert payload into public key, then extract the payload and compare to input
    CPubKey pubKey = CreatePubKey(ParseHex(strIn));
    BOOST_CHECK(pubKey.IsFullyValid());
    BOOST_CHECK_EQUAL(HexStr(ExtractPayload(pubKey)), strExpected);
}

BOOST_AUTO_TEST_CASE(payload_embedding_and_extraction_test)
{
    // Payloads are padded to 31 byte
    checkPubKeyPayload(
            "0011",
            "00110000000000000000000000000000000000000000000000000000000000");

    // Payloads with a size of 31 byte are not modified
    checkPubKeyPayload(
            "619c30f643a4679ec2f690f3d6564df7df2ae23ae4a55393ae0bef22db9dbc",
            "619c30f643a4679ec2f690f3d6564df7df2ae23ae4a55393ae0bef22db9dbc");

    // All bytes afterwards are ignored, when creating one public key
    checkPubKeyPayload(
            "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
            "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcd");
}

BOOST_AUTO_TEST_CASE(payload_size_test)
{
    // 30 byte payloads fit into one public key
    std::vector<CPubKey> vPubKeysPartialPacket;
    std::vector<unsigned char> vchPartialPacket = ParseHex(
            "010203040506070809101112131415161718192021222324252627282930");

    ConvertToPubKeys(vchPartialPacket, vPubKeysPartialPacket);
    BOOST_CHECK_EQUAL(1, vPubKeysPartialPacket.size());

    // 31 byte payloads also fit into one public key
    std::vector<CPubKey> vPubKeysFullPacket;
    std::vector<unsigned char> vchFullPacket = ParseHex(
            "01020304050607080910111213141516171819202122232425262728293031");

    ConvertToPubKeys(vchFullPacket, vPubKeysFullPacket);
    BOOST_CHECK_EQUAL(1, vPubKeysFullPacket.size());

    // 32 byte payloads require two public keys
    std::vector<CPubKey> vPubKeysMoreThanOnePacket;
    std::vector<unsigned char> vchMoreThanOnePacket = ParseHex(
            "01020304050607080910111213141516171819202122232425262728293031"
            "32");

    ConvertToPubKeys(vchMoreThanOnePacket, vPubKeysMoreThanOnePacket);
    BOOST_CHECK_EQUAL(2, vPubKeysMoreThanOnePacket.size());
}

BOOST_AUTO_TEST_CASE(convert_to_valid_pubkeys_test)
{
    const size_t nPacketSize = 31;
    const size_t nNumPubKeys = 255;
    const size_t nBytesTotal = nPacketSize * nNumPubKeys;

    std::vector<unsigned char> vchPayload;
    vchPayload.reserve(nBytesTotal);

    // Fill with random bytes
    for (size_t n = 0; n < nBytesTotal; ++n) {
        vchPayload.push_back(static_cast<unsigned char>(std::rand() % 256));
    }

    // Create the public keys
    std::vector<CPubKey> vPubKeys;
    ConvertToPubKeys(vchPayload, vPubKeys);
    BOOST_CHECK_EQUAL(vPubKeys.size(), nNumPubKeys);

    // Confirm ECDSA point validity
    std::vector<CPubKey>::const_iterator it;
    for (it = vPubKeys.begin(); it != vPubKeys.end(); ++it) {
        BOOST_CHECK(it->IsFullyValid());
    }

    // Extract and compare payload
    BOOST_CHECK(IsEqual(ExtractPayload(vPubKeys), vchPayload));
}

BOOST_AUTO_TEST_SUITE_END()
