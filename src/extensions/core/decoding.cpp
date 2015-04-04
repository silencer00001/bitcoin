#include "extensions/core/decoding.h"

#include "extensions/bitcoin/script.h"
#include "extensions/core/modifications/pubkeys.h"
#include "extensions/core/modifications/seqnums.h"
#include "extensions/core/modifications/uppersha256.h"
#include "extensions/log.h"

#include "primitives/transaction.h"
#include "pubkey.h"
#include "script/script.h"
#include "script/standard.h"

#include <string>
#include <vector>

/**
 * Extracts embedded payloads from bare multisig transaction outputs.
 *
 * @param vTxOuts  The obfuscated transaction outputs
 * @param vchPayloadRet The extracted payload
 * @return True if it was successful
 */
bool DecodeBareMultisig(const std::vector<CTxOut>& vTxOuts, std::vector<unsigned char>& vchPayloadRet)
{
    // TODO: this is very inconvenient
    // TODO: fail hard or continue, if not multisig?
    for (std::vector<CTxOut>::const_iterator it = vTxOuts.begin(); it != vTxOuts.end(); ++it) {
        if (GetOutputType(it->scriptPubKey) != TX_MULTISIG)
            return false;

        std::vector<std::vector<unsigned char> > vvchChucks;
        if (!GetScriptPushes(it->scriptPubKey, vvchChucks))
            return false;

        // Skip first public key!
        for (size_t n = 1; n < vvchChucks.size(); ++n) {
            CPubKey pubKey(vvchChucks[n].begin(), vvchChucks[n].end());
            std::vector<unsigned char> vchChuck = ExtractPayload(pubKey);
            vchPayloadRet.insert(vchPayloadRet.end(), vchChuck.begin(), vchChuck.end());
        }
    }

    return true;
}

/**
 * Extracts and deobfuscates embedded payloads from bare multisig transaction outputs.
 *
 * @param strSeed  The seed used for the deobfuscation
 * @param vTxOuts  The obfuscated transaction outputs
 * @param vchPayloadRet The extracted payload
 * @return True if it was successful
 */
bool DecodeBareMultisigObfuscated(const std::string& strSeed, const std::vector<CTxOut>& vTxOuts,
            std::vector<unsigned char>& vchPayloadRet)
{
    std::vector<unsigned char> vchPayload;
    if (!DecodeBareMultisig(vTxOuts, vchPayload))
        return false;

    DeobfuscateUpperSha256In(vchPayload, strSeed);
    RemoveSequenceNumbersIn(vchPayload);

    vchPayloadRet.insert(vchPayloadRet.end(), vchPayload.begin(), vchPayload.end());

    return true;
}
