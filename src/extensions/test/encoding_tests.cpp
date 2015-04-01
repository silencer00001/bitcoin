#include "extensions/core/encoding.h"
#include "extensions/test/test_utils.h"

#include "base58.h"
#include "primitives/transaction.h"
#include "pubkey.h"
#include "utilstrencodings.h"

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(encoding_tests)

BOOST_AUTO_TEST_CASE(encode_bare_multisig_test)
{
    CPubKey pubKey(ParseHex(
        "0347d08029b5cbc934f6079b650c50718eab5a56d51cf6b742ec9f865a41fcfca3"));
    std::vector<unsigned char> vchPayload(ParseHex(
        "00000000000000010000000002faf080"));

    std::vector<CTxOut> vTxOuts;
    BOOST_CHECK(EncodeBareMultisig(pubKey, vchPayload, vTxOuts));
    BOOST_CHECK_EQUAL(vTxOuts.size(), 1);
}

BOOST_AUTO_TEST_SUITE_END()
