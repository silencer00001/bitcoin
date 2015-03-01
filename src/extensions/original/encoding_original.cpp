/*
int EncodeMultisigOriginal(const std::string& strSeed, const CPubKey& redeemingPubKey, const std::vector<unsigned char>& vchPayload, std::vector<CTxOut>& vchTxOutRet)
{
    int nRemainingBytes = vchPayload.size();
    int nNextByte = 0;
    unsigned char seqNum = 1;

    std::string strObfuscatedHashes[1+MAX_SHA256_OBFUSCATION_TIMES];
    PrepareObfuscatedHashes(strSeed, strObfuscatedHashes);

    while (nRemainingBytes > 0)
    {
        int nKeys = 1;   // assume one key of data since we have data remaining
        if (nRemainingBytes > (PACKET_SIZE - 1))
            nKeys += 1;  // we have enough data for 2 keys in this output

        // always include the redeeming pubkey
        std::vector<CPubKey> keys;
        keys.push_back(redeemingPubKey);

        // append actual data chucks
        for (int i = 0; i < nKeys; i++)
        {
            std::vector<unsigned char> fakeKey;

            // add sequence number
            fakeKey.push_back(seqNum);
            
            // add up to 30 bytes of data
            int numBytes = nRemainingBytes < (PACKET_SIZE - 1) ? nRemainingBytes: (PACKET_SIZE - 1);
            fakeKey.insert(fakeKey.end(), vchPayload.begin() + nNextByte, vchPayload.begin() + nNextByte + numBytes);
            nNextByte += numBytes;
            nRemainingBytes -= numBytes;

            // pad to 31 total bytes with zeros
            while (fakeKey.size() < PACKET_SIZE) {
                fakeKey.push_back(0);
            }

            // xor in the obfuscation
            std::vector<unsigned char>hash = ParseHex(strObfuscatedHashes[seqNum]);
            for (int j = 0; j < PACKET_SIZE; j++) {
                fakeKey[j] = fakeKey[j] ^ hash[j];
            }

            // prepend the 2
            fakeKey.insert(fakeKey.begin(), 2);
            fakeKey.resize(33);

            // fix up the ecdsa code point
            CPubKey pubKey(fakeKey.begin(), fakeKey.end());
            if (!ModifyEcdsaPoint(pubKey)) {
                return error(-998, "no valid ecdsa point");
            }

            keys.push_back(pubKey);
            seqNum++;
        }

        CScript script = GetScriptForMultisig(1, keys);
        CTxOut txout(GetDustThreshold(script), script);

        vchTxOutRet.push_back(txout);

        // Dust value not consensus critical
        LogPrint("encoding", "multisig script: %s\n", script.ToString());
    }

    return success();
}
*/