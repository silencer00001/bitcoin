#include "extensions/core/encoding.h"

#include "pubkey.h"
#include "utilstrencodings.h"

#include <cstdio>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(ecdsa_modification_tests)

BOOST_AUTO_TEST_CASE(ecdsa_modfication)
{
    std::string str("02777ac9576cb08fb869efd4be2ca094c55808054e2220285f3c754d59361b3000");
    std::vector<unsigned char> vch(ParseHex(str));
    CPubKey pubKey(vch);
    BOOST_CHECK(ModifyEcdsaPoint(pubKey));
}

BOOST_AUTO_TEST_SUITE_END()
