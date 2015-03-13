#ifndef EXTENSIONS_CORE_ENCODING_H
#define EXTENSIONS_CORE_ENCODING_H

class CPubKey;
class CTxOut;

#include <string>
#include <vector>

/** Converts a stream of raw bytes into bare multisig transaction outputs. */
bool EncodeBareMultisig(const CPubKey& redeemingPubKey, const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vchTxOutsRet);
/** Converts a stream of raw bytes into obfuscated bare multisig transaction outputs. */
bool EncodeBareMultisigObfuscated(const std::string& strSeed, const CPubKey& redeemingPubKey, const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vchTxOutRet);
/** Converts a stream of raw bytes into OP_RETURN outputs. */
bool EncodeNullData(const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vchTxOutsRet, bool fOversize = false);

#endif // EXTENSIONS_CORE_ENCODING_H
