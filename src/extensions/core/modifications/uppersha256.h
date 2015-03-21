#ifndef EXTENSIONS_CORE_MODIFICATIONS_UPPERSHA256_H
#define EXTENSIONS_CORE_MODIFICATIONS_UPPERSHA256_H

#include <string>
#include <vector>

/** To be deprechiated. Used for compability of old functions. */
#ifndef MAX_SHA256_OBFUSCATION_TIMES
#define MAX_SHA256_OBFUSCATION_TIMES  255
#endif
/** To be deprechiated. Used for compability of old functions. */
void PrepareObfuscatedHashes(const std::string& strSeed, std::vector<std::string>& vstrHashes, size_t nCount);
/** To be deprechiated. Used for compability of old functions. */
void PrepareObfuscatedHashes(const std::string& strSeed, std::string(&pstrHashes)[1 + MAX_SHA256_OBFUSCATION_TIMES]);

/** Creates a SHA256 hash. */
std::string Sha256(const std::string& strInput);
/** Creates a SHA256 hash and transforms the hash as string to upper characters. */
std::string UpperSha256(const std::string& strInput);
/** Obfuscates a chuck of data by xor-ing it with another chuck. */
std::vector<unsigned char> XorHashMix(const std::string& strHash, const std::vector<unsigned char>& vchData);

/** Obfuscates a stream of raw bytes. */
std::vector<unsigned char> ObfuscateUpperSha256(const std::vector<unsigned char>& vch, const std::string& strSeed);
/** Deobfuscates a stream of raw bytes. */
std::vector<unsigned char> DeobfuscateUpperSha256(const std::vector<unsigned char>& vch, const std::string& strSeed);

/** Obfuscates a stream of raw bytes. */
void ObfuscateUpperSha256In(std::vector<unsigned char>& vch, const std::string& strSeed);
/** Deobfuscates a stream of raw bytes. */
void DeobfuscateUpperSha256In(std::vector<unsigned char>& vch, const std::string& strSeed);

#endif // EXTENSIONS_CORE_MODIFICATIONS_UPPERSHA256_H
