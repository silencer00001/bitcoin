#ifndef EXTENSIONS_CORE_ENCODING_H
#define EXTENSIONS_CORE_ENCODING_H

class CPubKey;
class CTxOut;

#include <string>
#include <vector>

/** Modifies the last byte of a public key until it's a valid ECDSA point. */
bool ModifyEcdsaPoint(CPubKey& vchPubKey);

/** Creates a compressed public key from raw bytes. */
bool CreatePubKey(const std::vector<unsigned char>& vch, CPubKey& pubKeyRet);

/** Converts a stream of bytes into compressed public keys. */
bool ConvertToPubKeys(const std::vector<unsigned char>& vch, std::vector<CPubKey>& vPubKeyRet);

/** Inserts sequence numbers into a stream of raw bytes. */
size_t AddSequenceNumbers(const std::vector<unsigned char>& vch, std::vector<unsigned char>& vchRet);

/** Obfuscates a chuck of data by xor-ing it with another chuck. */
void ObfuscatePacket(const std::string& strHash, std::vector<unsigned char>& vchData);

/** Obfuscates a stream of raw bytes. */
void ObfuscatePackets(const std::string& strSeed, const std::vector<unsigned char>& vch, std::vector<unsigned char>& vchRet);

/** Converts a stream of raw bytes into bare multisig transaction outputs. */
bool EncodeBareMultisig(const CPubKey& redeemingPubKey, const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vchTxOutsRet);

/** Converts a stream of raw bytes into obfuscated bare multisig transaction outputs. */
bool EncodeBareMultisigObfuscated(const std::string& strSeed, const CPubKey& redeemingPubKey, const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vchTxOutRet);

/** Converts a stream of raw bytes into OP_RETURN outputs. */
bool EncodeNullData(const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vchTxOutsRet, bool fOversize = false);

#endif // EXTENSIONS_CORE_ENCODING_H
