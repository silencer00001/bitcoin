#ifndef EXTENSIONS_CORE_OBFUSCATION_H
#define EXTENSIONS_CORE_OBFUSCATION_H

#include <string>

#define MAX_SHA256_OBFUSCATION_TIMES  255

void PrepareObfuscatedHashes(const std::string& strSeed, std::string(&ObfsHashes)[1 + MAX_SHA256_OBFUSCATION_TIMES]);

#endif // EXTENSIONS_CORE_OBFUSCATION_H
