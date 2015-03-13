#ifndef EXTENSIONS_CORE_MODIFICATIONS_PUBKEYS_H
#define EXTENSIONS_CORE_MODIFICATIONS_PUBKEYS_H

#include <vector>

class CPubKey;

/** Modifies the last byte of a public key until it's a valid ECDSA point. */
bool ModifyEcdsaPoint(CPubKey& vchPubKey);
/** Creates a compressed public key from raw bytes. */
bool CreatePubKey(const std::vector<unsigned char>& vch, CPubKey& pubKeyRet);
/** Converts a stream of bytes into compressed public keys. */
bool ConvertToPubKeys(const std::vector<unsigned char>& vch, std::vector<CPubKey>& vPubKeyRet);

#endif // EXTENSIONS_CORE_MODIFICATIONS_PUBKEYS_H
