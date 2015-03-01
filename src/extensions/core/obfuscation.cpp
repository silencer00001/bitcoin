#include "extensions/core/obfuscation.h"

#include "extensions/log.h"

#include "utilstrencodings.h"

#include <openssl/sha.h>

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

template<typename WritableRangeT>
inline void TransformToUpper(WritableRangeT& value) {
    std::transform(value.begin(), value.end(), value.begin(), ::toupper);
}

void PrepareObfuscatedHashes(const std::string& strSeed, std::string(&ObfsHashes)[1 + MAX_SHA256_OBFUSCATION_TIMES])
{
    const size_t sizeHash = 32u;
    const size_t sizeCopy = 65u;
    const size_t sizeInput = std::max(strlen(strSeed.c_str()) + 1u, sizeCopy);

    unsigned char shaInput[sizeInput];
    unsigned char shaResult[sizeHash];

    strcpy((char *) shaInput, strSeed.c_str());

    // do only as many re-hashes as there are mastercoin packets, 255 per spec
    for (size_t j = 1; j <= MAX_SHA256_OBFUSCATION_TIMES; ++j)
    {
        SHA256(shaInput, strlen((const char *) shaInput), shaResult);

        ObfsHashes[j] = HexStr(shaResult, shaResult + sizeHash);
        TransformToUpper(ObfsHashes[j]);

        LogPrint("msc_debug_verbose2", "%d: SHA256: %s\n", j, ObfsHashes[j]);

        memcpy(shaInput, ObfsHashes[j].c_str(), sizeCopy);
    }
}
