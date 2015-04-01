#ifndef EXTENSIONS_CORE_MODIFICATIONS_PACKETS_H
#define EXTENSIONS_CORE_MODIFICATIONS_PACKETS_H

#include <stddef.h>
#include <vector>

/**
 * Returns a part of a vector specified by start and end position.
 *
 * @param[in] vch     The vector
 * @param[in] nBegin  First position
 * @param[in] nEnd    Last position
 * @return The subvector
 */
template <typename T>
inline std::vector<T> Subrange(const std::vector<T>& vch, size_t nStart, size_t nEnd)
{
    const size_t nSize = vch.size();

    // Ensure it's not beyond the last element
    if (nEnd > nSize) {
        nEnd = nSize;
    }

    return std::vector<T>(vch.begin() + nStart, vch.begin() + nEnd);
}

/**
 * Slices a vector into parts of the given length.
 *
 * @param[in]  vch      The vector
 * @param[out] vvchRet  The subvectors
 * @param[in]  nLength  The length of each part
 * @return The number of subvectors
 */
template <typename T>
inline size_t Slice(const std::vector<T>& vch, std::vector<std::vector<T> >& vvchRet, size_t nLength)
{
    const size_t nTotal = vch.size();
    const size_t nItems = (nTotal / nLength) + (nTotal % nLength != 0);

    vvchRet.reserve(nItems);

    for (size_t nPos = 0; nPos < nTotal; nPos += nLength) {
        vvchRet.push_back(Subrange(vch, nPos, nPos + nLength));
    }

    return nItems;
}

/**
 * Pads a payload to the size of completely filled data packets.
 *
 * To ensure empty space is not filled with zeros during the public key conversion, the
 * payload should be padded before obfuscation, so that the fillers are also obfuscated.
 *
 * @param vchPayload[in,out]  The payload to modify
 * @param nPacketSize[in]     The packet size (defaults to 31 byte)
 */
template <typename T>
void PadBeforeObfuscationIn(std::vector<T>& vchPayload, size_t nPacketSize = 31)
{
    const size_t nSize = vchPayload.size();
    const size_t nPackets = (nSize / nPacketSize) + (nSize % nPacketSize != 0);

    vchPayload.resize(nPackets * nPacketSize);
}

#endif // EXTENSIONS_CORE_MODIFICATIONS_PACKETS_H
