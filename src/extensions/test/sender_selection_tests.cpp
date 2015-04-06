#include "extensions/core/transactions.h"

#include "base58.h"
#include "primitives/transaction.h"
#include "random.h"
#include "uint256.h"

#include <stdint.h>
#include <algorithm>
#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(sender_selection_tests)

static const unsigned nOutputs = 256;
static const unsigned nAllRounds = 2;
static const unsigned nShuffleRounds = 64;

static CTxOut createTxOut(int64_t amount, const std::string& dest)
{
    return CTxOut(amount, GetScriptForDestination(CBitcoinAddress(dest).Get()));
}

static CKeyID createRandomKeyId()
{
    std::vector<unsigned char> vch;
    vch.reserve(20);
    for (int i = 0; i < 20; ++i) {
        vch.push_back(static_cast<unsigned char>(std::rand() % 256));
    }
    return CKeyID(uint160(vch));
}

static CScriptID createRandomScriptId()
{
    std::vector<unsigned char> vch;
    vch.reserve(20);
    for (int i = 0; i < 20; ++i) {
        vch.push_back(static_cast<unsigned char>(std::rand() % 256));
    }
    return CKeyID(uint160(vch));
}

void shuffleAndCheck(std::vector<CTxOut>& vouts, unsigned nRounds)
{
    std::string strSenderFirst;
    BOOST_CHECK(GetSenderByContribution(vouts, strSenderFirst));

    for (unsigned j = 0; j < nRounds; ++j) {
        std::random_shuffle(vouts.begin(), vouts.end(), GetRandInt);

        std::string strSender;
        BOOST_CHECK(GetSenderByContribution(vouts, strSender));
        BOOST_CHECK_EQUAL(strSenderFirst, strSender);
    }
}

BOOST_AUTO_TEST_CASE(sender_selection_same_amount_test)
{
    for (unsigned i = 0; i < nAllRounds; ++i) {
        std::vector<CTxOut> vouts;
        for (unsigned n = 0; n < nOutputs; ++n) {
            CTxOut output(static_cast<int64_t>(1000),
                    GetScriptForDestination(createRandomKeyId()));
            vouts.push_back(output);
        }
        shuffleAndCheck(vouts, nShuffleRounds);
    }
}

BOOST_AUTO_TEST_CASE(sender_selection_increasing_amount_test)
{
    for (unsigned i = 0; i < nAllRounds; ++i) {
        std::vector<CTxOut> vouts;
        for (unsigned n = 0; n < nOutputs; ++n) {
            CTxOut output(static_cast<int64_t>(1000 + n),
                    GetScriptForDestination(createRandomKeyId()));
            vouts.push_back(output);
        }
        shuffleAndCheck(vouts, nShuffleRounds);
    }
}

BOOST_AUTO_TEST_CASE(sender_selection_mixed_test)
{
    for (unsigned i = 0; i < nAllRounds; ++i) {
        std::vector<CTxOut> vouts;
        for (unsigned n = 0; n < nOutputs; ++n) {
            CScript scriptPubKey;
            if (std::rand() % 2 == 0) {
                scriptPubKey = GetScriptForDestination(createRandomKeyId());
            } else {
                scriptPubKey = GetScriptForDestination(createRandomScriptId());
            };
            int64_t nAmount = static_cast<int64_t>(1000 - n * (n % 2 == 0));
            vouts.push_back(CTxOut(nAmount, scriptPubKey));
        }
        shuffleAndCheck(vouts, nShuffleRounds);
    }
}

BOOST_AUTO_TEST_CASE(p2pkh_contribution_by_sum_test)
{
    std::vector<CTxOut> vouts;
    vouts.push_back(createTxOut(100, "1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe"));
    vouts.push_back(createTxOut(100, "1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe"));
    vouts.push_back(createTxOut(100, "1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe"));
    vouts.push_back(createTxOut(100, "168F7az8ACWxntNqL1FathoKTbgK17GVft"));
    vouts.push_back(createTxOut(100, "168F7az8ACWxntNqL1FathoKTbgK17GVft"));
    vouts.push_back(createTxOut(999, "16X6UDz6dMkVAAkWdY6HKe85o6EVAbzDtn")); // Winner
    vouts.push_back(createTxOut(100, "1BYk8d1fWy1JLcNqHyNTEg2Jxu9EDb1BmY"));
    vouts.push_back(createTxOut(100, "1BYk8d1fWy1JLcNqHyNTEg2Jxu9EDb1BmY"));
    vouts.push_back(createTxOut(100, "1CE8bBr1dYZRMnpmyYsFEoexa1YoPz2mfB"));

    std::string strExpected("16X6UDz6dMkVAAkWdY6HKe85o6EVAbzDtn");

    for (int i = 0; i < 10; ++i) {
        std::random_shuffle(vouts.begin(), vouts.end(), GetRandInt);

        std::string strSender;
        BOOST_CHECK(GetSenderByContribution(vouts, strSender));
        BOOST_CHECK_EQUAL(strExpected, strSender);
    }
}

BOOST_AUTO_TEST_CASE(p2pkh_contribution_by_total_sum_test)
{
    std::vector<CTxOut> vouts;
    vouts.push_back(createTxOut(499, "1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe"));
    vouts.push_back(createTxOut(501, "1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe"));
    vouts.push_back(createTxOut(295, "1CE8bBr1dYZRMnpmyYsFEoexa1YoPz2mfB")); // Winner
    vouts.push_back(createTxOut(310, "1CE8bBr1dYZRMnpmyYsFEoexa1YoPz2mfB")); // Winner
    vouts.push_back(createTxOut(400, "1CE8bBr1dYZRMnpmyYsFEoexa1YoPz2mfB")); // Winner
    vouts.push_back(createTxOut(500, "1Pr75FNvtoWHeocNfc4zTQCfK5kMVakWcn"));
    vouts.push_back(createTxOut(500, "1Pr75FNvtoWHeocNfc4zTQCfK5kMVakWcn"));

    std::string strExpected("1CE8bBr1dYZRMnpmyYsFEoexa1YoPz2mfB");

    for (int i = 0; i < 1; ++i) {
        std::random_shuffle(vouts.begin(), vouts.end(), GetRandInt);

        std::string strSender;
        BOOST_CHECK(GetSenderByContribution(vouts, strSender));
        BOOST_CHECK_EQUAL(strExpected, strSender);
    }
}

BOOST_AUTO_TEST_CASE(p2pkh_contribution_by_sum_order_test)
{
    std::vector<CTxOut> vouts;
    vouts.push_back(createTxOut(1000, "1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe")); // Winner
    vouts.push_back(createTxOut(1000, "168F7az8ACWxntNqL1FathoKTbgK17GVft"));
    vouts.push_back(createTxOut(1000, "16X6UDz6dMkVAAkWdY6HKe85o6EVAbzDtn"));
    vouts.push_back(createTxOut(1000, "1BYk8d1fWy1JLcNqHyNTEg2Jxu9EDb1BmY"));
    vouts.push_back(createTxOut(1000, "1CE8bBr1dYZRMnpmyYsFEoexa1YoPz2mfB"));    
    vouts.push_back(createTxOut(1000, "1HG3s4Ext3sTqBTHrgftyUzG3cvx5ZbPCj"));
    vouts.push_back(createTxOut(1000, "1K6JtSvrHtyFmxdtGZyZEF7ydytTGqasNc"));    
    vouts.push_back(createTxOut(1000, "1Pa6zyqnhL6LDJtrkCMi9XmEDNHJ23ffEr"));
    vouts.push_back(createTxOut(1000, "1Pr75FNvtoWHeocNfc4zTQCfK5kMVakWcn"));

    std::string strExpected("1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe");

    for (int i = 0; i < 10; ++i) {
        std::random_shuffle(vouts.begin(), vouts.end(), GetRandInt);

        std::string strSender;
        BOOST_CHECK(GetSenderByContribution(vouts, strSender));
        BOOST_CHECK_EQUAL(strExpected, strSender);
    }
}

BOOST_AUTO_TEST_CASE(p2sh_contribution_by_sum_test)
{
    std::vector<CTxOut> vouts;
    vouts.push_back(createTxOut(100, "1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe"));
    vouts.push_back(createTxOut(150, "32LUY4obZmFGFVtJTs4o1qk4dxo1bYq9s8"));
    vouts.push_back(createTxOut(400, "34To5z7h35gQ54212gFRJSkwJUxGREr3SZ"));
    vouts.push_back(createTxOut(100, "1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe"));
    vouts.push_back(createTxOut(400, "3HsMqdiWqaziUe9VExna46NbWBAFaioMrK"));
    vouts.push_back(createTxOut(100, "1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe"));
    vouts.push_back(createTxOut(777, "3KYen723gbjhJU69j8jRhZU6fDw8iVVWKy")); // Winner
    vouts.push_back(createTxOut(100, "3KuUrmqdM7BWfHTsyyLyYnJ57HgnKurvkK"));

    std::string strExpected("3KYen723gbjhJU69j8jRhZU6fDw8iVVWKy");

    for (int i = 0; i < 10; ++i) {
        std::random_shuffle(vouts.begin(), vouts.end(), GetRandInt);

        std::string strSender;
        BOOST_CHECK(GetSenderByContribution(vouts, strSender));
        BOOST_CHECK_EQUAL(strExpected, strSender);
    }
}

BOOST_AUTO_TEST_CASE(p2sh_contribution_by_total_sum_test)
{
    std::vector<CTxOut> vouts;

    vouts.push_back(createTxOut(100, "34To5z7h35gQ54212gFRJSkwJUxGREr3SZ"));
    vouts.push_back(createTxOut(500, "34To5z7h35gQ54212gFRJSkwJUxGREr3SZ"));
    vouts.push_back(createTxOut(600, "3CD1QW6fjgTwKq3Pj97nty28WZAVkziNom")); // Winner
    vouts.push_back(createTxOut(500, "1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe"));
    vouts.push_back(createTxOut(100, "1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe"));
    vouts.push_back(createTxOut(350, "3CD1QW6fjgTwKq3Pj97nty28WZAVkziNom")); // Winner
    vouts.push_back(createTxOut(110, "1471EHpnJ62MDxLw96dKcNT8sWPEbHrAUe"));

    std::string strExpected("3CD1QW6fjgTwKq3Pj97nty28WZAVkziNom");

    for (int i = 0; i < 1; ++i) {
        std::random_shuffle(vouts.begin(), vouts.end(), GetRandInt);

        std::string strSender;
        BOOST_CHECK(GetSenderByContribution(vouts, strSender));
        BOOST_CHECK_EQUAL(strExpected, strSender);
    }
}

BOOST_AUTO_TEST_CASE(p2sh_contribution_by_sum_order_test)
{
    std::vector<CTxOut> vouts;
    vouts.push_back(createTxOut(1000, "32LUY4obZmFGFVtJTs4o1qk4dxo1bYq9s8")); // Winner
    vouts.push_back(createTxOut(1000, "34To5z7h35gQ54212gFRJSkwJUxGREr3SZ"));
    vouts.push_back(createTxOut(1000, "3CD1QW6fjgTwKq3Pj97nty28WZAVkziNom"));
    vouts.push_back(createTxOut(1000, "3GLZeQfjqFpULBzS12TjraZziNewnCE7bd"));
    vouts.push_back(createTxOut(1000, "3HsMqdiWqaziUe9VExna46NbWBAFaioMrK"));    
    vouts.push_back(createTxOut(1000, "3KYen723gbjhJU69j8jRhZU6fDw8iVVWKy"));
    vouts.push_back(createTxOut(1000, "3KuUrmqdM7BWfHTsyyLyYnJ57HgnKurvkK"));    
    vouts.push_back(createTxOut(1000, "3NukJ6fYZJ5Kk8bPjycAnruZkE5Q7UW7i8"));
    vouts.push_back(createTxOut(1000, "3PeDornjuSraASTqyN7WDMexTogz29QnXu"));    
    
    std::string strExpected("32LUY4obZmFGFVtJTs4o1qk4dxo1bYq9s8");

    for (int i = 0; i < 10; ++i) {
        std::random_shuffle(vouts.begin(), vouts.end(), GetRandInt);

        std::string strSender;
        BOOST_CHECK(GetSenderByContribution(vouts, strSender));
        BOOST_CHECK_EQUAL(strExpected, strSender);
    }
}

BOOST_AUTO_TEST_CASE(sender_selection_string_based_test)
{
    // Omni Core currently processes everything as string!
    // This has implications for the actual order.
    std::vector<CTxOut> vouts;
    vouts.push_back(createTxOut(1000, "18d49BjkbBCoK659fuktKjB3HugxK9NbpW")); // Winner
    vouts.push_back(createTxOut(1000, "1BDfBRCA3WBuHYHcaAdy4vojgmWZQncc16"));
    vouts.push_back(createTxOut(1000, "1HtkY9cvSBEQugk1R3XT5wjJZuyvSyzT8W"));
    vouts.push_back(createTxOut(1000, "1aWp2ceCw95EVBwAv8HwX4Gofx9ksthjv"));  
    
    std::string strExpected("18d49BjkbBCoK659fuktKjB3HugxK9NbpW");

    for (int i = 0; i < 24; ++i) {
        std::random_shuffle(vouts.begin(), vouts.end(), GetRandInt);

        std::string strSender;
        BOOST_CHECK(GetSenderByContribution(vouts, strSender));
        BOOST_CHECK_EQUAL(strExpected, strSender);
    }
}

BOOST_AUTO_TEST_SUITE_END()
