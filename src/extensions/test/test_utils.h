#ifndef EXTENSIONS_TEST_TEST_UTILS_H
#define EXTENSIONS_TEST_TEST_UTILS_H

#include "utilstrencodings.h"

#include <boost/test/unit_test.hpp>

#include <algorithm>

/** Checks whether two ranges are equal. */
template <class T>
inline bool IsEqual(const T& lhs, const T& rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    return (std::equal(lhs.begin(), lhs.end(), rhs.begin()) &&
           (std::equal(rhs.begin(), rhs.end(), lhs.begin())));
}

#define CHECK_COLLECTIONS(lhs, rhs) \
    BOOST_CHECK_EQUAL(HexStr(lhs), HexStr(rhs))

#define CHECK_COLLECTIONS_NE(lhs, rhs) \
    BOOST_CHECK(HexStr(lhs) != HexStr(rhs))

#endif // EXTENSIONS_TEST_TEST_UTILS_H
