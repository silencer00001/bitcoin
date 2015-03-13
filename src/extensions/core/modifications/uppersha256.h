#ifndef EXTENSIONS_CORE_MODIFICATIONS_UPPERSHA256_H
#define EXTENSIONS_CORE_MODIFICATIONS_UPPERSHA256_H

#include <string>
#include <vector>

std::vector<unsigned char> XorHashMix(const std::string& strHash, std::vector<unsigned char>& vchData);
std::string Sha256(const std::string& strInput);
std::string UpperSha256(const std::string& strInput);

/** To be deprechiated. */
#ifndef MAX_SHA256_OBFUSCATION_TIMES
#define MAX_SHA256_OBFUSCATION_TIMES  255
#endif
/** To be deprechiated. */
void PrepareObfuscatedHashes(const std::string& strSeed, std::vector<std::string>& vstrHashes, size_t nCount);
/** To be deprechiated. */
void PrepareObfuscatedHashes(const std::string& strSeed, std::string(&pstrHashes)[1 + MAX_SHA256_OBFUSCATION_TIMES]);

/** Obfuscates a stream of raw bytes. */
std::vector<unsigned char> ObfuscateUpperSha256(const std::vector<unsigned char>& vch, const std::string& strSeed);
/** Deobfuscates a stream of raw bytes. */
std::vector<unsigned char> DeobfuscateUpperSha256(const std::vector<unsigned char>& vch, const std::string& strSeed);

/** Obfuscates a stream of raw bytes. */
void ObfuscateUpperSha256In(std::vector<unsigned char>& vch, const std::string& strSeed);
/** Deobfuscates a stream of raw bytes. */
void DeobfuscateUpperSha256In(std::vector<unsigned char>& vch, const std::string& strSeed);

#endif // EXTENSIONS_CORE_MODIFICATIONS_UPPERSHA256_H
