#include "mastercore_parse_string.h"

#include <string>

#include <boost/test/unit_test.hpp>

using namespace mastercore;

BOOST_AUTO_TEST_SUITE(mastercore_strtoint64_tests)

BOOST_AUTO_TEST_CASE(mastercore_strtoint64_invidisible)
{
    // zero amount
    BOOST_CHECK(strToInt64("0", false) == 0);
    // big num
    BOOST_CHECK(strToInt64("4000000000000000", false) == 4000000000000000);
    // max int64
    BOOST_CHECK(strToInt64("9223372036854775807", false) == 9223372036854775807);
}

BOOST_AUTO_TEST_CASE(mastercore_strtoint64_invidisible_truncate)
{
    // ignore any char after decimal mark
    BOOST_CHECK(strToInt64("8.76543210123456878901", false) == 8);
    BOOST_CHECK(strToInt64("8.765432101.2345687890", false) == 8);
    BOOST_CHECK(strToInt64("2345.AbCdEhf71z1.23", false) == 2345);
}

BOOST_AUTO_TEST_CASE(mastercore_strtoint64_invidisible_invalid)
{
    // invalid, number is negative
    BOOST_CHECK(strToInt64("-4", false) == 0);
    // invalid, number is over max int64
    BOOST_CHECK(strToInt64("9223372036854775808", false) == 0);
    // invalid, minus sign in string
    BOOST_CHECK(strToInt64("2345.AbCdEFG71z88-1.23", false) == 0);
}

BOOST_AUTO_TEST_CASE(mastercore_strtoint64_divisible)
{
    // range 0 to max int64
    BOOST_CHECK(strToInt64("0.000", true) == 0);    
    BOOST_CHECK(strToInt64("92233720368.54775807", true) == 9223372036854775807);
    // check padding
    BOOST_CHECK(strToInt64("0.00000004", true) == 4);
    BOOST_CHECK(strToInt64("0.0000004", true) == 40);
    BOOST_CHECK(strToInt64("0.0004", true) == 40000);
    BOOST_CHECK(strToInt64("0.4", true) == 40000000);
    BOOST_CHECK(strToInt64("4.0", true) == 400000000);    
    // truncate after 8 digits
    BOOST_CHECK(strToInt64("40.00000000000099", true) == 4000000000);
    BOOST_CHECK(strToInt64("92233720368.54775807000", true) == 9223372036854775807);
}

BOOST_AUTO_TEST_CASE(mastercore_strtoint64_divisible_truncate)
{
    // truncate after 8 digits
    BOOST_CHECK(strToInt64("40.00000000000099", true) == 4000000000);
    BOOST_CHECK(strToInt64("92233720368.54775807000", true) == 9223372036854775807);
    BOOST_CHECK(strToInt64("92233720368.54775807000", true) == 9223372036854775807);
}

BOOST_AUTO_TEST_CASE(mastercore_strtoint64_divisible_invalid)
{
    // invalid, number is over max int64
    BOOST_CHECK(strToInt64("92233720368.54775808", true) == 0);
    // invalid, more than one decimal mark in string
    BOOST_CHECK(strToInt64("1234..12345678", true) == 0);
    // invalid, alpha chars in string
    BOOST_CHECK(strToInt64("1234.12345A", true) == 0);
    // invalid, number is negative
    BOOST_CHECK(strToInt64("-4.0", true) == 0);
    // invalid, minus sign in string
    BOOST_CHECK(strToInt64("4.1234-5678", true) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
