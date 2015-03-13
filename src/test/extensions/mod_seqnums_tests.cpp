#include "test/extensions/test_utils.h"

#include "extensions/core/modifications/seqnums.h"

#include "utilstrencodings.h"

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(mod_seqnums_tests)

static const size_t nPacketSize = 31;

BOOST_AUTO_TEST_CASE(seqnums_insertion_test)
{
    // Check default sequence number is 0x01
    BOOST_CHECK(IsEqual(
            AddSequenceNumbers(ParseHex("00"), (unsigned char) 0x01),
            AddSequenceNumbers(ParseHex("00"))));

    // Check the insertion increases the size by 1 byte
    BOOST_CHECK_EQUAL(
            AddSequenceNumbers(ParseHex("00")).size(),
            ParseHex("00").size() + (size_t) 1);

    // Check sequence number 0x02 is prepended
    BOOST_CHECK(IsEqual(
            AddSequenceNumbers(ParseHex("0304050607"), (unsigned char) 0x02),
            ParseHex("020304050607")));

    // Check sequence number 0xff is prepended
    BOOST_CHECK(IsEqual(
            AddSequenceNumbers(ParseHex("040815162342"), (unsigned char) 0xff),
            ParseHex("ff040815162342")));

    // Confirm one sequence number is prepended to a then whole packet
    BOOST_CHECK(IsEqual(
            AddSequenceNumbers(ParseHex(
            "000000000000000000000000000000000000000000000000000000000000")),
            ParseHex(
            "01000000000000000000000000000000000000000000000000000000000000")));

    // Confirm two sequence numbers are prepended
    BOOST_CHECK(IsEqual(
            AddSequenceNumbers(ParseHex(
            "000000000000000000000000000000000000000000000000000000000000"
            "00")),
            ParseHex(
            "01000000000000000000000000000000000000000000000000000000000000"
            "0200")));

    // Check sequence numbers 0x42 and 0x43 are prepended
    BOOST_CHECK(IsEqual(
            AddSequenceNumbers(ParseHex(
            "54686973206973206c6f6e676572207468616e206120726567756c617220"
            "64617461207061636b6167652e"), (unsigned char) 0x42),
            ParseHex(
            "4254686973206973206c6f6e676572207468616e206120726567756c617220"
            "4364617461207061636b6167652e")));
}

BOOST_AUTO_TEST_CASE(seqnums_removal_test)
{
    // Check sequence number 0x01 is removed
    BOOST_CHECK(IsEqual(
            RemoveSequenceNumbers(ParseHex("0100000000000000010000000005f5e100")),
            ParseHex("00000000000000010000000005f5e100")));

    // Check sequence number is removed from a whole packet
    BOOST_CHECK(IsEqual(
            RemoveSequenceNumbers(ParseHex(
            "01000000000000000000000000000000000000000000000000000000000000")),
            ParseHex(
            "000000000000000000000000000000000000000000000000000000000000")));

    // Check sequence numbers are removed from more than one packet
    BOOST_CHECK(IsEqual(
            RemoveSequenceNumbers(ParseHex(
            "01000000000000000000000000000000000000000000000000000000000000"
            "0200")),
            ParseHex(
            "000000000000000000000000000000000000000000000000000000000000"
            "00")));
}

BOOST_AUTO_TEST_CASE(seqnums_empty_test)
{
    std::vector<unsigned char> vchEmpty;
    BOOST_CHECK(vchEmpty.empty());
    BOOST_CHECK(!vchEmpty.size());

    // Check no sequence number is inserted into an empty stream
    BOOST_CHECK(AddSequenceNumbers(vchEmpty).empty());
    BOOST_CHECK(!AddSequenceNumbers(vchEmpty).size());

    // Check it's possible to remove one remaining element
    BOOST_CHECK(IsEqual(RemoveSequenceNumbers(ParseHex("01")), vchEmpty));

    // Check trying to remove sequence numbers from an empty stream is safe
    BOOST_CHECK_NO_THROW(RemoveSequenceNumbers(vchEmpty));
    BOOST_CHECK(IsEqual(RemoveSequenceNumbers(vchEmpty), vchEmpty));
    BOOST_CHECK(RemoveSequenceNumbers(vchEmpty).empty());
    BOOST_CHECK(!RemoveSequenceNumbers(vchEmpty).size());
}

BOOST_AUTO_TEST_CASE(seqnums_full_test)
{
    const unsigned char nFirstSeqNum = 1;
    const unsigned char nMaxSeqNum = 255;

    const size_t nSizeWithSeqNums = nPacketSize * nMaxSeqNum;
    const size_t nSizeWithoutSeqNums = nSizeWithSeqNums - nMaxSeqNum;

    // Stream without sequence numbers
    const std::vector<unsigned char> vchWithoutSeqNums(nSizeWithoutSeqNums, 0x00);
    BOOST_CHECK_EQUAL(vchWithoutSeqNums.size(), nSizeWithoutSeqNums);

    // Stream with sequence numbers
    const std::vector<unsigned char> vchWithSeqNums = AddSequenceNumbers(vchWithoutSeqNums);
    BOOST_CHECK_EQUAL(vchWithSeqNums.size(), nSizeWithSeqNums);

    // Check every position
    for (size_t nPos = 0; nPos < nSizeWithSeqNums; ++nPos) {
        if (nPos % nPacketSize == 0) {
            unsigned char nSeqNum = nFirstSeqNum + (nPos / nPacketSize);
            BOOST_CHECK_EQUAL(vchWithSeqNums[nPos], nSeqNum);
        } else {
            BOOST_CHECK_EQUAL(vchWithSeqNums[nPos], 0x00);
        }
    }

    // Remove all sequence numbers
    const std::vector<unsigned char> vchAfterRemoval = RemoveSequenceNumbers(vchWithSeqNums);

    // Confirm sequence numbers were removed
    BOOST_CHECK_EQUAL(vchAfterRemoval.size(), nSizeWithoutSeqNums);
    BOOST_CHECK(IsEqual(vchAfterRemoval, vchWithoutSeqNums));
}

BOOST_AUTO_TEST_CASE(seqnums_back_and_forth_test)
{
    const size_t nPacketChucks = 2;
    const size_t nPackets = nPacketChucks + 1;
    const size_t nOffset = 17;
    const size_t nPacketSize = 31;
    const unsigned char nFirstSeqNum = 0x05;

    const size_t nSizeWithSeqNums = nPacketSize * nPacketChucks + nOffset;
    const size_t nSizeWithoutSeqNums = nSizeWithSeqNums - nPackets;

    const std::vector<unsigned char> vchWithout(nSizeWithoutSeqNums, 0xff);
    std::vector<unsigned char> vchModified(vchWithout);

    vchModified = AddSequenceNumbers(vchModified, nFirstSeqNum);
    BOOST_CHECK(!IsEqual(vchWithout, vchModified));
    BOOST_CHECK_EQUAL(vchModified.at(0), nFirstSeqNum);
    BOOST_CHECK_EQUAL(vchModified.size(), nSizeWithSeqNums);

    vchModified = RemoveSequenceNumbers(vchModified);
    BOOST_CHECK(IsEqual(vchWithout, vchModified));

    AddSequenceNumbersIn(vchModified);
    BOOST_CHECK(!IsEqual(vchWithout, vchModified));
    BOOST_CHECK_EQUAL(vchModified.at(0), 0x01); // default first sequence number
    BOOST_CHECK_EQUAL(vchModified.size(), nSizeWithSeqNums);

    RemoveSequenceNumbersIn(vchModified);
    BOOST_CHECK(IsEqual(vchWithout, vchModified));

    BOOST_CHECK(IsEqual(vchModified, RemoveSequenceNumbers(AddSequenceNumbers(vchModified))));
    BOOST_CHECK(IsEqual(vchModified, vchWithout));
}

BOOST_AUTO_TEST_SUITE_END()
