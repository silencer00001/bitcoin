#include "extensions/core/modifications/uppersha256.h"

#include "crypto/sha256.h"
#include "utilstrencodings.h"

#include <algorithm>
#include <string>
#include <vector>

static const size_t gMaxSha256ObfuscationTimes = 255;

static const size_t gPacketSize = 31;

/**
 * Obfuscates a chuck of data by xor-ing it with another chuck.
 *
 * @param[in]     strHash  A string of data
 * @param[in,out] vchData  A vector of data
 */
std::vector<unsigned char> XorHashMix(const std::string& strHash, std::vector<unsigned char>& vchData)
{
    const size_t nLength = gPacketSize;

    std::vector<unsigned char> vchHash = ParseHex(strHash);
    vchHash.resize(nLength);
    vchData.resize(nLength);

    // Xor the data
    for (size_t n = 0; n < nLength; ++n) {
        vchHash[n] = vchHash[n] ^ vchData[n];
    }

    return vchHash;
}

std::string Sha256(const std::string& strInput)
{
    unsigned char pHash[CSHA256::OUTPUT_SIZE];
    const unsigned char* pData = reinterpret_cast<const unsigned char*>(strInput.c_str());
    CSHA256().Write(pData, strInput.size()).Finalize(pHash);

    return HexStr(pHash, pHash + CSHA256::OUTPUT_SIZE);
}

std::string UpperSha256(const std::string& strInput)
{
    std::string strHash = Sha256(strInput);
    std::transform(strHash.begin(), strHash.end(), strHash.begin(), ::toupper);

    return strHash;
}

void PrepareObfuscatedHashes(const std::string& strSeed, std::vector<std::string>& vstrHashes, size_t nCount)
{
    std::string strHash(strSeed);
    vstrHashes.reserve(nCount);

    for (size_t n = 0; n < nCount; ++n) {
        strHash = UpperSha256(strHash);
        vstrHashes.push_back(strHash);
    }
}

void PrepareObfuscatedHashes(const std::string& strSeed, std::string(&pstrHashes)[1 + gMaxSha256ObfuscationTimes])
{
    std::vector<std::string> vstrHashes;
    PrepareObfuscatedHashes(strSeed, vstrHashes, gMaxSha256ObfuscationTimes);
    std::copy(vstrHashes.begin(), vstrHashes.end(), pstrHashes + 1);
}

/**
 * Obfuscates a stream of raw bytes.
 *
 * @param[in] vch      The raw data
 * @param[in] strSeed  A seed used for the obfuscation
 * @return The obfuscated data
 */
std::vector<unsigned char> ObfuscateUpperSha256(const std::vector<unsigned char>& vch, const std::string& strSeed)
{
    const size_t nLength = gPacketSize;

    std::string strHash(strSeed);
    std::vector<unsigned char> vchRet;

    const size_t nTotal = vch.size();
    const size_t nItems = (nTotal / nLength) + (nTotal % nLength != 0);

    vchRet.reserve(nTotal + nItems);

    for (size_t n = 0; n < nItems; ++n)
    {
        size_t nPos = n * nLength;
        size_t nEnd = std::min(nPos + nLength, nTotal);

        std::vector<unsigned char> vchData(vch.begin() + nPos, vch.begin() + nEnd);

        strHash = UpperSha256(strHash);
        vchData = XorHashMix(strHash, vchData);

        vchRet.insert(vchRet.end(), vchData.begin(), vchData.end());
    }

    return vchRet;
}

/**
 * Deobfuscates a stream of raw bytes.
 *
 * @param[in] vch      The raw data
 * @param[in] strSeed  A seed used for the deobfuscation
 * @return The obfuscated data
 */
std::vector<unsigned char> DeobfuscateUpperSha256(const std::vector<unsigned char>& vch, const std::string& strSeed)
{    
    return ObfuscateUpperSha256(vch, strSeed);
}

/**
 * Obfuscates a stream of raw bytes.
 *
 * @param[in,out] vch  The raw data to modify
 * @param[in] strSeed  A seed used for the obfuscation
 */
void ObfuscateUpperSha256In(std::vector<unsigned char>& vch, const std::string& strSeed)
{
    ObfuscateUpperSha256(vch, strSeed).swap(vch);
}

/**
 * Deobfuscates a stream of raw bytes.
 *
 * @param[in,out] vch  The raw data to modify
 * @param[in] strSeed  A seed used for the deobfuscation
 */
void DeobfuscateUpperSha256In(std::vector<unsigned char>& vch, const std::string& strSeed)
{
    DeobfuscateUpperSha256(vch, strSeed).swap(vch);
}
