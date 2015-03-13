#include "extensions/core/modifications/pubkeys.h"

#include "extensions/core/modifications/slice.h"

#include "pubkey.h"

#include <cstdlib>
#include <vector>

static const size_t gPacketSize = 31;

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
    while (!vchPubKey.IsFullyValid())
    {
        vchFakeKey[32] = (vchFakeKey[32] + 1) % 256;
        vchPubKey.Set(vchFakeKey.begin(), vchFakeKey.end());

        // Fail safe: stop if we cycled once
        if (vchFakeKey[32] == nPubKeyNonce)
        { return false; }
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
    if (!ModifyEcdsaPoint(pubKeyRet))
    { return false; }

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
    // [1 byte, prefix] [31 byte, packet] [1 byte, ecdsa byte]
    const size_t nLength = gPacketSize;

    // Slice the stream of bytes into chucks of data
    std::vector<std::vector<unsigned char> > vvchRet;
    Slice(vch, vvchRet, nLength);

    // Convert each chuck into a public key
    std::vector<std::vector<unsigned char> >::const_iterator it;
    for (it = vvchRet.begin(); it != vvchRet.end(); ++it)
    {
        CPubKey key;
        if (!CreatePubKey((*it), key))
        { return false; } // Failed to form a valid ECDSA point

        vPubKeyRet.push_back(key);
    }

    return true;
}
