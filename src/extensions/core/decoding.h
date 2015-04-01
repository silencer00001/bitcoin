#ifndef EXTENSIONS_CORE_DECODING_H
#define EXTENSIONS_CORE_DECODING_H

#include <string>
#include <vector>

class CTxOut;

/** Extracts embedded payloads from bare multisig transaction outputs. */
bool DecodeBareMultisig(const std::vector<CTxOut>& vTxOuts, std::vector<unsigned char>& vchPayloadRet);
/** Extracts and deobfuscates embedded payloads from bare multisig transaction outputs. */
bool DecodeBareMultisigObfuscated(const std::string& strSeed, const std::vector<CTxOut>& vTxOuts, std::vector<unsigned char>& vchPayloadRet);

#endif // EXTENSIONS_CORE_DECODING_H
