#include "extensions/core/modifications/pubkeys.h"

#include "extensions/core/modifications/slice.h"

#include "pubkey.h"

#include <cstdlib>
#include <vector>

static const size_t gPacketSize = 31;

/**
 * Modifies the last byte of a public key until it's a valid ECDSA coordinate.
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
        if (vchFakeKey[32] == nPubKeyNonce) { return false; }
    }

    return true;
}

/**
 * Creates a compressed public key from raw bytes.
 *
 * It is not guaranteed that the public key represents a valid ECDSA coordinate.
 *
 * @param vchPayload  The payload or data chuck
 * @return The public key
 */
CPubKey CreatePubKeyUnchecked(const std::vector<unsigned char>& vchPayload)
{
    // Vector for the public key
    std::vector<unsigned char> vchFakeKey;
    vchFakeKey.reserve(33);

    // Public key prefix (0x02, 0x03)
    unsigned char nPubKeyPrefix = 2 + (std::rand() & 1);

    // Prepare the public key
    vchFakeKey.insert(vchFakeKey.end(), nPubKeyPrefix);
    vchFakeKey.insert(vchFakeKey.end(), vchPayload.begin(), vchPayload.end());
    vchFakeKey.resize(33);

    // Create the public key
    CPubKey pubKey(vchFakeKey.begin(), vchFakeKey.end());

    return pubKey;
}

/**
 * Creates a fully valid compressed public key from raw bytes.
 *
 * A random public key prefix (0x02, 0x03) is prepended and the last byte of the
 * key is modified, until the public key represents a valid ECDSA coordinate.
 *
 * @param vchPayload  The payload or data chuck
 * @return The public key
 */
CPubKey CreatePubKey(const std::vector<unsigned char>& vchPayload)
{
    // Use random byte as seed, if none was given
    if (vchPayload.size() < 32) {
        std::vector<unsigned char> vchPayloadCopy = vchPayload;
        vchPayloadCopy.resize(32);
        vchPayloadCopy[31] = std::rand() % 256;

        return CreatePubKey(vchPayloadCopy);
    }

    CPubKey pubKey = CreatePubKeyUnchecked(vchPayload);
    ModifyEcdsaPoint(pubKey);

    return pubKey;
}

/**
 * Converts a stream of bytes into compressed public keys.
 *
 * @param[in]  vchPayload  The payload or raw data
 * @param[out] vPubKeyRet  The public keys
 */
void ConvertToPubKeys(const std::vector<unsigned char>& vchPayload, std::vector<CPubKey>& vPubKeysRet)
{
    // [1 byte, prefix] [31 byte, packet] [1 byte, ecdsa byte]
    const size_t nLength = gPacketSize;

    // Slice the stream of bytes into chucks of data
    std::vector<std::vector<unsigned char> > vvchChucks;
    Slice(vchPayload, vvchChucks, nLength);

    // Convert each chuck into a public key
    std::vector<std::vector<unsigned char> >::const_iterator it;
    for (it = vvchChucks.begin(); it != vvchChucks.end(); ++it) {
        CPubKey pubKey = CreatePubKey(*it);
        vPubKeysRet.push_back(pubKey);
    }
}

/**
 * Extracts and returns the embedded payload of a public key.
 *
 * The public key prefix and last byte of the public key are trimmed.
 *
 * @param vchPubKey  The public key
 * @return The payload
 */
std::vector<unsigned char> ExtractPayload(const CPubKey& vchPubKey)
{
    // Trim prefix and ECDSA point manipulation byte
    return std::vector<unsigned char>(vchPubKey.begin() + 1, vchPubKey.end() - 1);
}

/**
 * Extracts and returns the embedded payload of the given public keys.
 *
 * The public key prefix and last byte of each public key are trimmed. The chucks
 * of data are concatenated.
 *
 * @param vPubKeys  The public keys
 * @return The payload
 */
std::vector<unsigned char> ExtractPayload(const std::vector<CPubKey>& vPubKeys)
{
    const size_t nPayloadSize = vPubKeys.size() * gPacketSize;

    std::vector<unsigned char> vchPayload;
    vchPayload.reserve(nPayloadSize);

    std::vector<CPubKey>::const_iterator it;
    for (it = vPubKeys.begin(); it != vPubKeys.end(); ++it) {
        std::vector<unsigned char> vchChuck = ExtractPayload(*it);
        vchPayload.insert(vchPayload.end(), vchChuck.begin(), vchChuck.end());
    }

    return vchPayload;
}
