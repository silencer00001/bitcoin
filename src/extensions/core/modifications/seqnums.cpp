#include "extensions/core/modifications/seqnums.h"

#include <stddef.h>
#include <vector>

static const size_t gPacketSize = 31;

/**
 * Inserts sequence numbers into a stream of raw bytes.
 *
 * @param[in] vch           The raw data
 * @param[in] nFirstSeqNum  The first sequence number
 * @return The raw data with sequence numbers
 */
std::vector<unsigned char> AddSequenceNumbers(const std::vector<unsigned char>& vch, unsigned char nFirstSeqNum)
{
    const size_t nLength = gPacketSize - 1;

    std::vector<unsigned char> vchRet;

    const size_t nTotal = vch.size();
    const size_t nItems = (nTotal / nLength) + (nTotal % nLength != 0);

    vchRet.reserve(nTotal + nItems);

    for (size_t n = 0; n < nItems; ++n)
    {
        unsigned char nSeqNum = n + nFirstSeqNum;

        size_t nPos = n * nLength;
        size_t nEnd = std::min(nPos + nLength, nTotal);

        vchRet.insert(vchRet.end(), nSeqNum);
        vchRet.insert(vchRet.end(), vch.begin() + nPos, vch.begin() + nEnd);
    }

    return vchRet;
}

/**
 * Removes sequence numbers from a stream of raw bytes.
 *
 * @param[in] vch  The raw data
 * @return The raw data without sequence numbers
 */
std::vector<unsigned char> RemoveSequenceNumbers(const std::vector<unsigned char>& vch)
{
    const size_t nLength = gPacketSize;

    std::vector<unsigned char> vchRet;

    const size_t nTotal = vch.size();
    const size_t nItems = (nTotal / nLength) + (nTotal % nLength != 0);

    vchRet.reserve(nTotal - nItems);

    for (size_t n = 0; n < nItems; ++n)
    {
        size_t nPos = n * nLength;
        size_t nEnd = std::min(nPos + nLength, nTotal);

        vchRet.insert(vchRet.end(), vch.begin() + nPos + 1, vch.begin() + nEnd);
    }

    return vchRet;
}

/**
 * Inserts sequence numbers into a stream of raw bytes.
 *
 * @param[in,out] vch       The raw data to modify
 * @param[in] nFirstSeqNum  The first sequence number
 */
void AddSequenceNumbersIn(std::vector<unsigned char>& vch, unsigned char nFirstSeqNum)
{
    AddSequenceNumbers(vch, nFirstSeqNum).swap(vch);
}

/**
 * Removes sequence numbers from a stream of raw bytes.
 *
 * @param[in,out] vch  The raw data to modify
 */
void RemoveSequenceNumbersIn(std::vector<unsigned char>& vch)
{
    RemoveSequenceNumbers(vch).swap(vch);
}
