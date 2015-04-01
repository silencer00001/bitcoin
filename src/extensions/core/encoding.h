#ifndef EXTENSIONS_CORE_ENCODING_H
#define EXTENSIONS_CORE_ENCODING_H

class CPubKey;
class CTxOut;

#include "script/standard.h"

#include <string>
#include <vector>

/** Converts a stream of raw bytes into bare multisig transaction outputs. */
bool EncodeBareMultisig(const CPubKey& redeemingPubKey, const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vTxOutsRet);
/** Converts a stream of raw bytes into obfuscated bare multisig transaction outputs. */
bool EncodeBareMultisigObfuscated(const std::string& strSeed, const CPubKey& redeemingPubKey, const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vTxOutRet);
/** Converts a stream of raw bytes into OP_RETURN outputs. */
bool EncodeNullData(const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vTxOutsRet, size_t nMaxSize = nMaxDatacarrierBytes);

#endif // EXTENSIONS_CORE_ENCODING_H
