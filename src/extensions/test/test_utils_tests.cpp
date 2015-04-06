#include "extensions/test/test_utils.h"

#include "utilstrencodings.h"

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(test_utils_tests)

BOOST_AUTO_TEST_CASE(string_comparison_test)
{
    BOOST_CHECK_EQUAL("test", "test");
    BOOST_CHECK(std::string("test").compare("test") == 0);
    BOOST_CHECK(std::string("test").compare("abcd") != 0);
    BOOST_CHECK(std::string("abcd").compare("test") != 0);    
}

BOOST_AUTO_TEST_CASE(hex_conversion_test)
{
    std::vector<unsigned char> vch;
    vch.push_back(static_cast<unsigned char>(0x00));
    vch.push_back(static_cast<unsigned char>(0x01));
    vch.push_back(static_cast<unsigned char>(0x23));
    vch.push_back(static_cast<unsigned char>(0x42));
    vch.push_back(static_cast<unsigned char>(0xff));

    // Check HexStr() converts a vector to a hex string
    BOOST_CHECK_EQUAL(HexStr(vch), "00012342ff");
    BOOST_CHECK_EQUAL("00012342ff", HexStr(vch));

    // Check back and forth conversion
    BOOST_CHECK_EQUAL(HexStr(ParseHex(HexStr(vch))), HexStr(vch));
    BOOST_CHECK_EQUAL(HexStr(ParseHex(HexStr(vch))), "00012342ff");

    // Confirm inequality
    BOOST_CHECK(HexStr(vch).compare("ff222342ff") != 0);
    BOOST_CHECK(HexStr(vch).compare("not equal!") != 0);
}

BOOST_AUTO_TEST_CASE(is_equal_string_test)
{
    // Check IsEqual() is true when comparing equal strings
    BOOST_CHECK(IsEqual(std::string(""), std::string("")));
    BOOST_CHECK(IsEqual(std::string("test"), std::string("test")));

    // Check IsEqual() is false when comparing unequal strings
    BOOST_CHECK(!IsEqual(std::string(""), std::string("fg")));
    BOOST_CHECK(!IsEqual(std::string("fg"), std::string("")));
    BOOST_CHECK(!IsEqual(std::string("abcd"), std::string("dcab")));
    BOOST_CHECK(!IsEqual(std::string("true"), std::string("false")));
}

BOOST_AUTO_TEST_CASE(is_equal_vector_test)
{
    // Check IsEqual() is true when comparing equal vectors
    BOOST_CHECK(IsEqual(std::vector<unsigned char>(), std::vector<unsigned char>()));
    BOOST_CHECK(IsEqual(ParseHex("0102030405060708"), ParseHex("0102030405060708")));

    // Check IsEqual() is false when comparing unequal vectors
    BOOST_CHECK(!IsEqual(ParseHex("00"), ParseHex("11")));
    BOOST_CHECK(!IsEqual(ParseHex("11"), ParseHex("00")));
    BOOST_CHECK(!IsEqual(ParseHex("00"), ParseHex("0000")));
}

BOOST_AUTO_TEST_CASE(check_collections_vector_test)
{
    // Check is true when comparing equal vectors
    CHECK_COLLECTIONS(std::vector<unsigned char>(), std::vector<unsigned char>());
    CHECK_COLLECTIONS(ParseHex("0102030405060708"), ParseHex("0102030405060708"));

    // Check is false when comparing unequal vectors
    CHECK_COLLECTIONS_NE(ParseHex("00"), ParseHex("11"));
    CHECK_COLLECTIONS_NE(ParseHex("11"), ParseHex("00"));
    CHECK_COLLECTIONS_NE(ParseHex("00"), ParseHex("0000"));
}

BOOST_AUTO_TEST_CASE(fill_vector_test)
{
    // Check vector generation by filling with 1 byte
    BOOST_CHECK_EQUAL(
            HexStr(std::vector<unsigned char>((size_t) 1, (unsigned char) 0x00)),
            "00");

    // Check vector generation by filling with 33 byte
    BOOST_CHECK_EQUAL(
            HexStr(std::vector<unsigned char>((size_t) 33, (unsigned char) 0x11)),
            "111111111111111111111111111111111111111111111111111111111111111111");

    // Confirm size of generated vector, filled with 8417 byte
    BOOST_CHECK_EQUAL(
            (size_t) 8417,
            std::vector<unsigned char>((size_t) 8417, (unsigned char) 0xff).size());
}

BOOST_AUTO_TEST_CASE(vector_generation_conversion_test)
{
    // Check filling works fine with auto converted values
    BOOST_CHECK_EQUAL(HexStr(std::vector<unsigned char>(1, 0)), "00");
    BOOST_CHECK_EQUAL(HexStr(std::vector<unsigned char>(1, 0xff)), "ff");
    BOOST_CHECK_EQUAL(HexStr(std::vector<unsigned char>(3, 5)), "050505");
    BOOST_CHECK_EQUAL(HexStr(std::vector<unsigned char>(2, 255)), "ffff");
}

BOOST_AUTO_TEST_SUITE_END()
