#include "test/extensions/test_utils.h"

#include "extensions/core/modifications/pubkeys.h"

#include "pubkey.h"
#include "utilstrencodings.h"

#include <cstdlib>
#include <string>
#include <vector>

#include <stdio.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(mod_pubkeys_tests)

BOOST_AUTO_TEST_CASE(ecdsa_modfication_test)
{
    // Invalid public key
    std::string strPubKey("02777ac9576cb08fb869efd4be2ca094c55808054e2220285f3c754d59361b3007");
    
    // Valid, modified public key
    std::string strPubKeyExpected("02777ac9576cb08fb869efd4be2ca094c55808054e2220285f3c754d59361b300c");

    CPubKey pubKey(ParseHex(strPubKey));
    CPubKey pubKeyExpected(ParseHex(strPubKeyExpected));

    BOOST_CHECK(!pubKey.IsFullyValid());
    BOOST_CHECK(ModifyEcdsaPoint(pubKey));
    BOOST_CHECK(pubKey.IsFullyValid());
    BOOST_CHECK(IsEqual(pubKey, pubKeyExpected));
}

BOOST_AUTO_TEST_CASE(create_pubkey_test)
{
    // Reset random number generator
    std::srand(1);

    CPubKey pubKey;
    std::vector<unsigned char> vchBlob = ParseHex("123456");

    std::string strPubKeyExpected("031234560000000000000000000000000000000000000000000000000000000001");
    CPubKey pubKeyExpected(ParseHex(strPubKeyExpected));

    BOOST_CHECK(!pubKey.IsFullyValid());
    BOOST_CHECK(CreatePubKey(vchBlob, pubKey));
    BOOST_CHECK(pubKey.IsFullyValid());
    BOOST_CHECK(IsEqual(pubKey, pubKeyExpected));
}

BOOST_AUTO_TEST_SUITE_END()
