#include "extensions/core/modifications/pubkeys.h"

#include "pubkey.h"
#include "utilstrencodings.h"

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(mod_pubkeys_tests)

BOOST_AUTO_TEST_CASE(ecdsa_modfication_test)
{
    // Invalid public key
    std::string str("02777ac9576cb08fb869efd4be2ca094c55808054e2220285f3c754d59361b3007");
    std::vector<unsigned char> vch(ParseHex(str));
    CPubKey pubKey(vch);

    BOOST_CHECK(!pubKey.IsFullyValid());
    BOOST_CHECK(ModifyEcdsaPoint(pubKey));
    BOOST_CHECK(pubKey.IsFullyValid());
}

BOOST_AUTO_TEST_SUITE_END()
