#ifndef EXTENSIONS_CORE_MODIFICATIONS_PUBKEYS_H
#define EXTENSIONS_CORE_MODIFICATIONS_PUBKEYS_H

#include <vector>

class CPubKey;

/** Modifies the last byte of a public key until it's a valid ECDSA coordinate. */
bool ModifyEcdsaPoint(CPubKey& vchPubKey);
/** Creates a compressed public key from raw bytes. */
CPubKey CreatePubKeyUnchecked(const std::vector<unsigned char>& vchPayload);
/** Creates a fully valid compressed public key from raw bytes. */
CPubKey CreatePubKey(const std::vector<unsigned char>& vchPayload);
/** Converts a stream of bytes into compressed public keys. */
void ConvertToPubKeys(const std::vector<unsigned char>& vchPayload, std::vector<CPubKey>& vPubKeysRet);
/** Extracts and returns the embedded payload of a public key. */
std::vector<unsigned char> ExtractPayload(const CPubKey& vchPubKey);
/** Extracts and returns the embedded payload of the given public keys. */
std::vector<unsigned char> ExtractPayload(const std::vector<CPubKey>& vPubKeys);

#endif // EXTENSIONS_CORE_MODIFICATIONS_PUBKEYS_H
