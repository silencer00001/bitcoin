#include "extensions/core/encoding.h"

#include "extensions/bitcoin/script.h"
#include "extensions/core/obfuscation.h"
#include "extensions/log.h"

#include "base58.h"
#include "pubkey.h"
#include "script/script.h"
#include "script/standard.h"

#include <cstdlib>
#include <string>
#include <vector>

static const size_t PACKET_SIZE = 31;

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
inline size_t Slice(const std::vector<T>& vch, std::vector<std::vector<T> >& vvchRet, size_t nLength = PACKET_SIZE)
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
 * Modifies the last byte of a public key until it's a valid ECDSA point.
 *
 * @param[in,out] vchPubKey  The public key to modify
 * @return True if a valid ECDSA point was found
 */
bool ModifyEcdsaPoint(CPubKey& vchPubKey)
{
    unsigned char nPubKeyNonce = vchPubKey[32];
    std::vector<unsigned char> vchFakeKey(vchPubKey.begin(), vchPubKey.end());

    // Modify the last byte until the it's a valid public key
    while (!vchPubKey.IsFullyValid()) {
        vchFakeKey[32] = (vchFakeKey[32] + 1) % 256;
        vchPubKey.Set(vchFakeKey.begin(), vchFakeKey.end());

        // Fail safe: stop if we cycled once
        if (vchFakeKey[32] == nPubKeyNonce) {
            return false;
        }
    }

    return true;
}

/**
 * Creates a compressed public key from raw bytes.
 *
 * @param[in]  vch        The data chuck
 * @param[out] pubKeyRet  The public key
 * @return True if a fully valid public key was created
 */
bool CreatePubKey(const std::vector<unsigned char>& vch, CPubKey& pubKeyRet)
{
    // Vector for the public key
    std::vector<unsigned char> vchFakeKey;
    vchFakeKey.reserve(33);

    // Public key prefix (0x02, 0x03)
    unsigned char nPubKeyPrefix = 2 + (std::rand() & 1);

    // Prepare the public key
    vchFakeKey.insert(vchFakeKey.end(), nPubKeyPrefix);
    vchFakeKey.insert(vchFakeKey.end(), vch.begin(), vch.end());
    vchFakeKey.resize(33);
 
    // Create the public key
    pubKeyRet.Set(vchFakeKey.begin(), vchFakeKey.end());

    // Modify last byte to form a valid ecdsa point
    if (!ModifyEcdsaPoint(pubKeyRet)) {
        LogPrint("encoding", "no valid ecdsa point found");
        return false;
    }

    return true;
}

/**
 * Converts a stream of bytes into compressed public keys.
 *
 * @param[in]  vch         The raw data
 * @param[out] vPubKeyRet  The public keys
 * @return True if the conversion was successful
 */
bool ConvertToPubKeys(const std::vector<unsigned char>& vch, std::vector<CPubKey>& vPubKeyRet)
{
    const size_t nLength = PACKET_SIZE;

    // Slice the stream of bytes into chucks of data
    std::vector<std::vector<unsigned char> > vvchRet;
    Slice(vch, vvchRet, nLength);

    // Convert each chuck into a public key
    std::vector<std::vector<unsigned char> >::const_iterator it;
    for (it = vvchRet.begin(); it != vvchRet.end(); ++it)
    {
        CPubKey key;
        if (!CreatePubKey((*it), key)) {
            return false; // Failed to form a valid ECDSA point
        }
        vPubKeyRet.push_back(key);
    }

    return true;
}

/**
 * Inserts sequence numbers into a stream of raw bytes.
 *
 * @param[in]  vch     The raw data
 * @param[out] vchRet  The raw data with sequence numbers
 * @return The number of sequence numbers
 */
size_t AddSequenceNumbers(const std::vector<unsigned char>& vch, std::vector<unsigned char>& vchRet)
{
    const size_t nPacket = PACKET_SIZE;
    const size_t nLength = nPacket - 1;

    // Slice the stream of bytes into chucks of data
    std::vector<std::vector<unsigned char> > vvchRet;
    size_t nNumPacket = Slice(vch, vvchRet, nLength);

    // Prepend each chuck with a sequence number and concatenate them
    for (size_t n = 0; n < nNumPacket; ++n) {
        unsigned char nSeqNum = n + 1;

        vchRet.insert(vchRet.end(), nSeqNum);
        vchRet.insert(vchRet.end(), vvchRet[n].begin(), vvchRet[n].end());
    }

    return nNumPacket;
}

/**
 * Obfuscates a chuck of data by xor-ing it with another chuck.
 *
 * @param[in]     strHash  A string of data
 * @param[in,out] vchData  A vector of data
 */
void ObfuscatePacket(const std::string& strHash, std::vector<unsigned char>& vchData)
{
    const size_t nLength = PACKET_SIZE;

    std::vector<unsigned char> vchHash = ParseHex(strHash);
    vchHash.resize(nLength);
    vchData.resize(nLength);

    // Xor the data
    for (size_t n = 0; n < nLength; ++n) {
        vchData[n] = vchHash[n] ^ vchData[n];
    }
}

/**
 * Obfuscates a stream of raw bytes.
 *
 * @param[in]  strSeed  A seed used for the obfuscation
 * @param[in]  vch      The raw data
 * @param[out] vchRet   The obfuscated data
 */
void ObfuscatePackets(const std::string& strSeed, const std::vector<unsigned char>& vch, std::vector<unsigned char>& vchRet)
{
    const size_t nLength = PACKET_SIZE;
    const size_t nHashes = 1+MAX_SHA256_OBFUSCATION_TIMES;

    // Create hashes used for obfuscation
    std::string vstrObfuscatedHashes[nHashes];
    PrepareObfuscatedHashes(strSeed, vstrObfuscatedHashes);

    // Slice the stream of bytes into chucks of data
    std::vector<std::vector<unsigned char> > vvchRet;
    size_t nNumPacket = Slice(vch, vvchRet, nLength);

    // Obfuscate each chuck and concatenate them to a new stream
    for (size_t n = 0; n < nNumPacket; ++n) {
        std::vector<unsigned char>& vchData = vvchRet[n];
        const std::string& strHash = vstrObfuscatedHashes[n+1];

        ObfuscatePacket(strHash, vchData);
        vchRet.insert(vchRet.end(), vchData.begin(), vchData.end());
    }
}

/**
 * Converts a stream of raw bytes into bare multisig transaction outputs.
 *
 * @param[in]  redeemingPubKey  A public key which should be used to spent the outputs
 * @param[in]  vchPayload       The raw data
 * @param[out] vchTxOutsRet     The transaction outputs
 * @return True if it was successful
 */
bool EncodeBareMultisig(const CPubKey& redeemingPubKey, const std::vector<unsigned char>& vchPayload,
            std::vector<CTxOut>& vchTxOutsRet)
{
    const size_t nMaxKeys = 3;

    // Transform the raw data into public keys
    std::vector<CPubKey> vPubKeys;
    ConvertToPubKeys(vchPayload, vPubKeys);

    // Create packs of public keys, used for each output
    std::vector<std::vector<CPubKey> > vvPubKeyPacks;
    Slice(vPubKeys, vvPubKeyPacks, (nMaxKeys - 1));

    // Insert the redeeming public key at the beginning of each multisig output, followed by the payload,
    // embedded as fully valid public keys, with up to nMaxKeys public keys per transaction output
    std::vector<std::vector<CPubKey> >::const_iterator it;
    for (it = vvPubKeyPacks.begin(); it != vvPubKeyPacks.end(); ++it)
    {
        std::vector<CPubKey> keys;
        keys.insert(keys.end(), redeemingPubKey);
        keys.insert(keys.end(), (*it).begin(), (*it).end());

        CScript script = GetScriptForMultisig(1, keys);
        CTxOut txout(GetDustThreshold(script), script);

        vchTxOutsRet.insert(vchTxOutsRet.end(), txout);
        LogPrint("encoding", "multisig script: %s\n", script.ToString());                
    }

    return true;
}

/**
 * Converts a stream of raw bytes into obfuscated bare multisig transaction outputs.
 *
 * @param[in]  strSeed          A seed used for the obfuscation
 * @param[in]  redeemingPubKey  A key which should be used to spent the outputs
 * @param[in]  vchPayload       The raw data
 * @param[out] vchTxOutRet      The obfuscated transaction outputs
 * @return True if it was successful
 */
bool EncodeBareMultisigObfuscated(const std::string& strSeed, const CPubKey& redeemingPubKey,
            const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vchTxOutRet)
{
    std::vector<unsigned char> vchSeqNum;
    std::vector<unsigned char> vchHashed;

    AddSequenceNumbers(vchPayload, vchSeqNum);    
    ObfuscatePackets(strSeed, vchSeqNum, vchHashed);

    return EncodeBareMultisig(redeemingPubKey, vchHashed, vchTxOutRet);
}

/**
 * Converts a stream of raw bytes into OP_RETURN outputs.
 *
 * @param[in]  vchPayload   The raw data
 * @param[out] vchTxOutRet  The transaction outputs
 * @param[in]  fOversize    Whether the client's data carrier size setting should be respected
 * @return True if it was successful
 */
bool EncodeNullData(const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vchTxOutsRet, bool fOversize)
{
    const size_t nSize = vchPayload.size();

    if (!fOversize && nSize > nMaxDatacarrierBytes) {
        return false;
    }

    CScript script;
    script << OP_RETURN << vchPayload;

    CTxOut txout(0, script);
    vchTxOutsRet.insert(vchTxOutsRet.end(), txout);

    return true;
}
