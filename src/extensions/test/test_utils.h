#ifndef EXTENSIONS_TEST_TEST_UTILS_H
#define EXTENSIONS_TEST_TEST_UTILS_H

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

#endif // EXTENSIONS_TEST_TEST_UTILS_H
