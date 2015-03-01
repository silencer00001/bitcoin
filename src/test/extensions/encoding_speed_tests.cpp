#include "extensions/core/encoding.h"

#include "pubkey.h"
#include "primitives/transaction.h"
#include "utilstrencodings.h"
#include "utiltime.h"
#include "leveldb/slice.h"

#include <cstdio>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#define TOTAL_ROUNDS 1000

BOOST_AUTO_TEST_SUITE(encoding_speed_tests)

static const std::string strPayload(
            "52687a61727c5bbec40061d464ee9d26f3528a29bf0f09cbb89469b45ac10bc0"
            "c95eec04be3bb8ea3aae264c940633bdc0dbeb09e8660f3a9952b601386c824d"
            "01b1de1d7e54702297f3150f811c9294b1f7f275b916f4273881d8c189ce295c"
            "e3954f7479c0afad9299341fdac286d511443e9863a00f749c18c6fc4db78845"
            "f6e243f51ea6dbb369eb9c5e999dbf4767789fdc32410d2f710710111ad7c49f"
            "eef563a39d9a547b049375d03e2ca9b50db5e7d91358d49b16b2bf1beb83ec51"
            "619e6a998f33c3813fff0727615a0db2631d7b2d2bd10d298e2c880015249850"
            "00badceae2bae09952db3e8e36dd738429ddcf2555ef30a7698847aa42f29d1b"
            "304ddfad40ceb9c78c7c2552ad3223d2886ff830e1954df5be8678fb13e062d5"
            "9375b205129832caf4853948c4c5cbe7994e062d8b2a9a8a6ddce255c21e6f26"
            "21b88cf7b814a4c9c3e7960a2347c54d3eadb74d5b002c8bebe6ff7295bf9bdc"
            "501cd6bcf2fde3b5a3223ecd0eb399af5b2ad946e397b433e7ce72861acf8640"
            "2cec5cf7ce3da96eec40b46014abb50afebd25bbf5f6a8f820180dc0d70f0d82"
            "15ab6aba91870272d16680cd0d2d2214fdacb82ad7846d3dd493139b62fcf8c1"
            "24113c7d635b987cc4ca5947779e6b41eb820515300db7752ffcc8f9ca3047aa"
            "a26325e7acd387c14744efe7749ac1427491a5f1d9992413280648a06d72bcba"
            "60e9333e133db4c76c3039e516c1a938cf27004c32836774fd5431cbeb0aa8d2"
            "c9e864b845bf246fabb0f1d25545fbde3be2b3431899e77992b6b16422d6cdc5"
            "331d27467f35f169fc6aa76ee6fe61e1ccd37e80f8f72e6fef1a4eb11f98f1e5"
            "a6e92876e63a0c0e0c38372be246bf7191cec3f11e9f4b01fad9daa766dbf967"
            "fc4f45203f22752319fb0b66c3f8bfbea8c61de5f31d10178ad035231e12fe61"
            "43d9ea400504c32c7b3cbeeeda5ce68ee9fed73e19beedd33fa05b174f6cea7c"
            "deda0f8777df61bc903a8f97aebe563ed73adda54161de97dfbae86cb40c7283"
            "46601cfbd700a767c163d31ea56b2ee8d9798a47444e5075c0bd63d7d7f8a9ea"
            "7fcb0aa8e66d6e655282b43182759ef880e80f368a26fc7489e36f833f8ae03b"
            "582cadcd21bb3dafe05eca2b57455fb8e0380eab806f80e9fada099ba2a74554"
            "5df8e3b56a325efd54560e8e8f7a1c82f9a70e55a9e61062b4411ed4aa2196aa"
            "a802f08b7f761b08bd90720f1c020b234d300f4bbf95dc2c525a6faacc613a94"
            "28925216f1c26ea8560df836d5885954920d78d397b460e082ea52e012238736"
            "ada7b14b4578cc6201b4a2aa3fba33decb28bb79ed4d0bbccc7ba4d939cf5afc");

static const std::vector<unsigned char> vchPayload(ParseHex(strPayload));
static const std::string strSeed("mwsPqf2ztrddFqNWYeQLsVuNLRCBicvvF5");
static const std::string strPubKey("023cccde2f3f47555dafcd7501e8c45f6fc1c7da494a657e7cc8a7c5d932058fa2");
static const CPubKey redeemingPubKey(ParseHex(strPubKey));
static const size_t nLength = 31;

/*
// Slice(Vector, Vector<Vector>, size_t)
BOOST_AUTO_TEST_CASE(encode_slice)
{
    size_t nResult = 0;
    size_t nRounds = TOTAL_ROUNDS * 100;

    int64_t nTime = GetTimeMicros();

    for (size_t i = 0; i < nRounds; ++i) {
        std::vector<std::vector<unsigned char> > vvchRet;
        nResult += Slice(vchPayload, vvchRet, nLength);
    }

    int64_t nTimeNow = GetTimeMicros();

    printf("Slice:                              \t Rounds: %lu \t Time: %.3f ms (%.3f ms/round)\n",
            nRounds,
            0.001 * (nTimeNow - nTime),
            0.001 * (nTimeNow - nTime) / nRounds);

    BOOST_CHECK_EQUAL(nResult, 31 * nRounds);
}
*/

// ConvertToPubKeys(Vector, Vec<CPubKey>)
BOOST_AUTO_TEST_CASE(encode_create_pubkeys)
{
    size_t nResult = 0;
    size_t nRounds = TOTAL_ROUNDS;

    int64_t nTime = GetTimeMicros();

    for (size_t i = 0; i < nRounds; ++i) {
        std::vector<CPubKey> vPubKeyRet;        
        nResult += ConvertToPubKeys(vchPayload, vPubKeyRet);
    }

    int64_t nTimeNow = GetTimeMicros();

    printf("Convert To Public Keys:             \t Rounds: %lu \t Time: %.3f ms (%.3f ms/round)\n",
            nRounds,
            0.001 * (nTimeNow - nTime),
            0.001 * (nTimeNow - nTime) / nRounds);

    BOOST_CHECK_EQUAL(nResult, 0);
}

// EncodeBareMultisig(CPubKey, Vector, Vec<CTxOut>)
BOOST_AUTO_TEST_CASE(encode_bare_multisig)
{
    size_t nResult = 0;
    size_t nRounds = TOTAL_ROUNDS;

    int64_t nTime = GetTimeMicros();

    for (size_t i = 0; i < nRounds; ++i) {
        std::vector<CTxOut> vchTxOutsRet;
        nResult += EncodeBareMultisig(redeemingPubKey, vchPayload, vchTxOutsRet);
    }

    int64_t nTimeNow = GetTimeMicros();

    printf("Encode Bare Multisig:               \t Rounds: %lu \t Time: %.3f ms (%.3f ms/round)\n",
            nRounds,
            0.001 * (nTimeNow - nTime),
            0.001 * (nTimeNow - nTime) / nRounds);

    BOOST_CHECK_EQUAL(nResult, 0);
}

// EncodeBareMultisigObfuscated(string, CPubKey, Vector, Vec<CTxOut>)
BOOST_AUTO_TEST_CASE(encode_bare_multisig_obfuscated)
{
    size_t nResult = 0;
    size_t nRounds = TOTAL_ROUNDS;

    int64_t nTime = GetTimeMicros();

    for (size_t i = 0; i < nRounds; ++i) {
        std::vector<CTxOut> vchTxOutsRet;
        nResult += EncodeBareMultisigObfuscated(strSeed, redeemingPubKey, vchPayload, vchTxOutsRet);
    }

    int64_t nTimeNow = GetTimeMicros();

    printf("Encode Bare Multisig Obfuscated:    \t Rounds: %lu \t Time: %.3f ms (%.3f ms/round)\n",
            nRounds,
            0.001 * (nTimeNow - nTime),
            0.001 * (nTimeNow - nTime) / nRounds);

    BOOST_CHECK_EQUAL(nResult, 0);
}

// EncodeMultisigOriginal(string, CPubKey, Vector, Vec<CTxOut>)
BOOST_AUTO_TEST_CASE(encode_bare_multisig_original)
{
    size_t nResult = 0;
    size_t nRounds = TOTAL_ROUNDS;

    int64_t nTime = GetTimeMicros();

    for (size_t i = 0; i < nRounds; ++i) {
        std::vector<CTxOut> vchTxOutsRet;
        nResult += EncodeMultisigOriginal(strSeed, redeemingPubKey, vchPayload, vchTxOutsRet);
    }

    int64_t nTimeNow = GetTimeMicros();

    printf("Encode Bare Multisig Original:      \t Rounds: %lu \t Time: %.3f ms (%.3f ms/round)\n",
            nRounds,
            0.001 * (nTimeNow - nTime),
            0.001 * (nTimeNow - nTime) / nRounds);

    BOOST_CHECK_EQUAL(nResult, 0);
}

BOOST_AUTO_TEST_CASE(encode_check_original_vs_new)
{
    std::vector<CTxOut> vchTxOuts;
    EncodeBareMultisigObfuscated(strSeed, redeemingPubKey, vchPayload, vchTxOuts);

    std::vector<CTxOut> vchTxOutsOriginal;
    EncodeMultisigOriginal(strSeed, redeemingPubKey, vchPayload, vchTxOutsOriginal);

    BOOST_CHECK_EQUAL(vchTxOuts.size(), vchTxOutsOriginal.size());
    
    std::vector<CTxOut>::const_iterator pc1 = vchTxOuts.begin();
    std::vector<CTxOut>::const_iterator pc2 = vchTxOutsOriginal.begin();

    while (pc1 != vchTxOuts.end() && pc2 != vchTxOutsOriginal.end())
    {
        const CTxOut& out1 = *pc1;
        const CTxOut& out2 = *pc2;

        BOOST_CHECK_EQUAL(out1.nValue, out2.nValue);
        BOOST_CHECK_EQUAL(HexStr(out1.scriptPubKey), HexStr(out1.scriptPubKey));

        pc1++;
        pc2++;
    }
}

BOOST_AUTO_TEST_SUITE_END()
