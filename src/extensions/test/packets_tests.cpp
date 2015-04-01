#include "extensions/core/modifications/packets.h"

#include "utilstrencodings.h"

#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(packet_tests)

BOOST_AUTO_TEST_CASE(vector_slice_test)
{
    size_t nNumChucks = 0;

    std::vector<std::vector<unsigned char> > vvchExact;
    nNumChucks = Slice(ParseHex("11223344"), vvchExact, 2);

    // Check vector slicing into two equally sized chucks
    BOOST_CHECK_EQUAL(vvchExact.size(), nNumChucks);
    BOOST_CHECK_EQUAL(HexStr(vvchExact[0]), "1122");
    BOOST_CHECK_EQUAL(HexStr(vvchExact[1]), "3344");

    std::vector<std::vector<unsigned char> > vvchExcess;
    nNumChucks = Slice(ParseHex("aabbcc"), vvchExcess, 2);

    // Check vector slicing into three chucks
    BOOST_CHECK_EQUAL(vvchExcess.size(), nNumChucks);
    BOOST_CHECK_EQUAL(HexStr(vvchExcess[0]), "aabb");
    BOOST_CHECK_EQUAL(HexStr(vvchExcess[1]), "cc");
}

BOOST_AUTO_TEST_CASE(packet_padding_test)
{
    // Fill vector with 16 0x77 (= 16 byte total)
    std::vector<unsigned char> vch(16, (unsigned char) 0x77);
    BOOST_CHECK_EQUAL(16, vch.size());
    BOOST_CHECK_EQUAL(HexStr(vch), "77777777777777777777777777777777");

    // Pad with a default packet size of 31 byte (= 31 byte total)
    PadBeforeObfuscationIn(vch);
    BOOST_CHECK_EQUAL(31, vch.size());
    BOOST_CHECK_EQUAL(HexStr(vch),
            "77777777777777777777777777777777000000000000000000000000000000");

    // Pad to a size of a multiple of 30 byte (= 60 byte total)
    PadBeforeObfuscationIn(vch, 30);
    BOOST_CHECK_EQUAL(60, vch.size());
    BOOST_CHECK_EQUAL(HexStr(vch),
            "777777777777777777777777777777770000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000");

    // Add one byte.. (= 61 byte total)
    vch.push_back((unsigned char) 0x44);
    BOOST_CHECK_EQUAL(61, vch.size());
    BOOST_CHECK_EQUAL(HexStr(vch),
            "777777777777777777777777777777770000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000"
            "44");

    // ..then pad with a packet size of 30 byte once more (= 90 byte total)
    PadBeforeObfuscationIn(vch, 30);
    BOOST_CHECK_EQUAL(90, vch.size());
    BOOST_CHECK_EQUAL(HexStr(vch),
            "777777777777777777777777777777770000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000"
            "440000000000000000000000000000000000000000000000000000000000");
}

BOOST_AUTO_TEST_SUITE_END()
