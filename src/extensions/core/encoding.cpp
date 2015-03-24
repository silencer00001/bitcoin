#include "extensions/core/encoding.h"

#include "extensions/bitcoin/script.h"
#include "extensions/core/modifications/pubkeys.h"
#include "extensions/core/modifications/seqnums.h"
#include "extensions/core/modifications/slice.h"
#include "extensions/core/modifications/uppersha256.h"
#include "extensions/log.h"

#include "base58.h"
#include "primitives/transaction.h"
#include "pubkey.h"
#include "script/script.h"
#include "script/standard.h"

#include <string>
#include <vector>

/**
 * Converts a stream of raw bytes into bare multisig transaction outputs.
 *
 * @param[in]  redeemingPubKey  A public key which should be used to spent the outputs
 * @param[in]  vchPayload       The raw data
 * @param[out] vchTxOutsRet     The transaction outputs
 * @return True if it was successful
 */
bool EncodeBareMultisig(const CPubKey& redeemingPubKey, const std::vector<unsigned char>& vchPayload,
            std::vector<CTxOut>& vTxOutsRet)
{
    const size_t nMaxKeys = 3;

    // Transform the raw data into public keys
    std::vector<CPubKey> vPubKeys;
    ConvertToPubKeys(vchPayload, vPubKeys);

    // Create packs of public keys, used for each output
    std::vector<std::vector<CPubKey> > vvPubKeyPacks;
    Slice(vPubKeys, vvPubKeyPacks, (nMaxKeys - 1));

    // Insert the redeeming public key at the beginning of each multisig output, followed by the payload,
    // embedded as fully valid public keys, with up to nMaxKeys public keys per transaction output
    std::vector<std::vector<CPubKey> >::const_iterator it;
    for (it = vvPubKeyPacks.begin(); it != vvPubKeyPacks.end(); ++it)
    {
        std::vector<CPubKey> keys;
        keys.insert(keys.end(), redeemingPubKey);
        keys.insert(keys.end(), (*it).begin(), (*it).end());

        CScript script = GetScriptForMultisig(1, keys);
        CTxOut txout(GetDustThreshold(script), script);

        vTxOutsRet.insert(vTxOutsRet.end(), txout);
        LogPrint("encoding", "multisig script: %s\n", script.ToString());
    }

    return true;
}

/**
 * Pads the payload to the size of completely filled data packets.
 *
 * To ensure empty space is not filled with zeros during the public key conversion, the
 * payload should be padded before obfuscation, so that the fillers are also obfuscated.
 *
 * @param vchPayload[in,out]  The payload to modify
 * @param nPacketSize[in]     The packet size (defaults to 31 byte)
 */
static void PadBeforeObfuscationIn(std::vector<unsigned char>& vchPayload, size_t nPacketSize = 31)
{
    const size_t nSize = vchPayload.size();
    const size_t nPackets = (nSize / nPacketSize) + (nSize % nPacketSize != 0);

    vchPayload.resize(nPackets * nPacketSize);
}

/**
 * Converts a stream of raw bytes into obfuscated bare multisig transaction outputs.
 *
 * @param[in]  strSeed          A seed used for the obfuscation
 * @param[in]  redeemingPubKey  A key which should be used to spent the outputs
 * @param[in]  vchPayload       The raw data
 * @param[out] vchTxOutRet      The obfuscated transaction outputs
 * @return True if it was successful
 */
bool EncodeBareMultisigObfuscated(const std::string& strSeed, const CPubKey& redeemingPubKey,
            const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vTxOutRet)
{
    std::vector<unsigned char> vchPayloadCopy = vchPayload;

    AddSequenceNumbersIn(vchPayloadCopy);
    PadBeforeObfuscationIn(vchPayloadCopy);
    ObfuscateUpperSha256In(vchPayloadCopy, strSeed);

    return EncodeBareMultisig(redeemingPubKey, vchPayloadCopy, vTxOutRet);
}

/**
 * Converts a stream of raw bytes into OP_RETURN outputs.
 *
 * @param[in]  vchPayload   The raw data
 * @param[out] vchTxOutRet  The transaction outputs
 * @param[in]  fOversize    Whether the client's data carrier size setting should be respected
 * @return True if it was successful
 */
bool EncodeNullData(const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vTxOutsRet, bool fOversize)
{
    const size_t nSize = vchPayload.size();

    if (!fOversize && nSize > nMaxDatacarrierBytes) {
        return false;
    }

    CScript script;
    script << OP_RETURN << vchPayload;

    CTxOut txout(0, script);

    vTxOutsRet.insert(vTxOutsRet.end(), txout);
    LogPrint("encoding", "nulldata script: %s\n", script.ToString());

    return true;
}
