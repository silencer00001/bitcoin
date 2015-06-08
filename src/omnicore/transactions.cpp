#include "omnicore/transactions.h"

#include "omnicore/encoding.h"
#include "omnicore/errors.h"
#include "omnicore/log.h"
#include "omnicore/omnicore.h"
#include "omnicore/script.h"

#include "base58.h"
#include "coincontrol.h"
#include "core_io.h"
#include "init.h"
#include "primitives/transaction.h"
#include "pubkey.h"
#include "script/script.h"
#include "script/standard.h"
#include "sync.h"
#include "uint256.h"
#include "util.h"
#include "wallet.h"

#include <stdint.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace mastercore
{

static bool GetTransactionInputs(const std::vector<COutPoint>& txInputs, std::vector<CTxOut>& txOutputsRet)
{
    txOutputsRet.clear();
    txOutputsRet.reserve(txInputs.size());

    // Gather outputs for all transaction inputs  
    for (std::vector<COutPoint>::const_iterator it = txInputs.begin(); it != txInputs.end(); ++it) {
        const COutPoint& outpoint = *it;

        CTransaction prevTx;
        uint256 prevTxBlock;
        if (!GetTransaction(outpoint.hash, prevTx, prevTxBlock, false)) {
            PrintToConsole("%s() ERROR: get transaction failed\n", __func__);
            return false;
        }
        CTxOut prevTxOut = prevTx.vout[outpoint.n];
        txOutputsRet.push_back(prevTxOut);
    }
    // Sanity check
    return (txInputs.size() == txOutputsRet.size());
}

static bool CheckInput(const CTxOut& txOut, int nHeight, CTxDestination& dest)
{
    txnouttype whichType;

    if (!GetOutputType(txOut.scriptPubKey, whichType)) {
        return false;
    }
    if (!IsAllowedOutputType(whichType, nHeight)) {
        return false;
    }
    if (!ExtractDestination(txOut.scriptPubKey, dest)) {
        return false;
    }

    return true;
}

static bool CheckTransactionInputs(const std::vector<CTxOut>& txInputs, std::string& strSender, int64_t& amountIn, int nHeight=9999999)
{
    amountIn = 0;
    strSender.clear();

    for (std::vector<CTxOut>::const_iterator it = txInputs.begin(); it != txInputs.end(); ++it) {
        const CTxOut& txOut = *it;

        CTxDestination dest;
        if (!CheckInput(txOut, nHeight, dest)) {
            PrintToConsole("%s() ERROR: transaction input not accepted\n", __func__);
            return false;
        }

        CBitcoinAddress address(dest);
        if (strSender.empty()) {
            strSender = address.ToString();
        }
        if (strSender != address.ToString()) {
            PrintToConsole("%s() ERROR: inputs must refer to the same address\n", __func__);
            return false;
        }

        amountIn += txOut.nValue;
    }

    return true;
}

static int64_t SelectCoins(const std::string& fromAddress, CCoinControl& coinControl, int64_t additional) {
    CWallet* pwallet = pwalletMain;
    if (!pwallet) { return 0; }

    int64_t n_max = (COIN * (20 * (0.0001))); // assume 20 kB max tx size at 0.0001 per kB
    int64_t n_total = 0; // total output funds collected

    // if referenceamount is set it is needed to be accounted for here too
    if (0 < additional) n_max += additional;

    int nHeight = GetHeight();
    LOCK2(cs_main, pwallet->cs_wallet);

    for (std::map<uint256, CWalletTx>::iterator it = pwallet->mapWallet.begin();
            it != pwallet->mapWallet.end(); ++it) {
        const uint256& wtxid = it->first;
        const CWalletTx* pcoin = &(*it).second;

        if (pcoin->IsTrusted()) {
            for (unsigned int n = 0; n < pcoin->vout.size(); n++) {
                const CTxOut& txOut = pcoin->vout[n];

                CTxDestination dest;
                if (!CheckInput(txOut, nHeight, dest)) {
                    continue;
                }
                if (!IsMine(*pwallet, dest)) {
                    continue;
                }
                if (pwallet->IsSpent(wtxid, n)) {
                    continue;
                }
                std::string sAddress = CBitcoinAddress(dest).ToString();
                if (msc_debug_tokens)
                    PrintToLog("%s:IsMine()=yes:IsSpent()=no:%s: n=%d, nValue=%d\n",
                        sAddress, wtxid.ToString(), n, txOut.nValue);

                // only use funds from the sender's address
                if (fromAddress == sAddress) {
                    COutPoint outpoint(wtxid, n);
                    coinControl.Select(outpoint);

                    n_total += txOut.nValue;

                    if (n_max <= n_total) {
                        break;
                    }
                }
            }
        }

        if (n_max <= n_total) {
            break;
        }
    }

    return n_total;
}

static bool AddressToPubKey(const std::string& sender, CPubKey& pubKey)
{
    CWallet* pwallet = pwalletMain;
    if (!pwallet) { return false; }

    CKeyID keyID;
    CBitcoinAddress address(sender);

    if (address.IsScript()) {
        PrintToLog("%s() ERROR: redemption address %s must not be a script hash\n", __func__, sender);
        return false;
    }
    if (!address.GetKeyID(keyID)) {
        PrintToLog("%s() ERROR: redemption address %s is invalid\n", __func__, sender);
        return false;
    }
    if (!pwallet->GetPubKey(keyID, pubKey)) {
        PrintToLog("%s() ERROR: failed to retrieve public key for redemption address %s from wallet\n", __func__, sender);
        return false;
    }
    if (!pubKey.IsFullyValid()) {
        PrintToLog("%s() ERROR: retrieved invalid public key for redemption address %s\n", __func__, sender);
        return false;
    }

    return true;
}

// This function determines whether it is valid to use a Class C transaction for a given payload size
static bool UseEncodingClassC(size_t nDataSize)
{
    size_t nTotalSize = nDataSize + 2; // Marker "om"
    bool fDataEnabled = GetBoolArg("-datacarrier", true);
    int nBlockNow = GetHeight();
    if (!IsAllowedOutputType(TX_NULL_DATA, nBlockNow)) {
        fDataEnabled = false;
    }
    return nTotalSize <= nMaxDatacarrierBytes && fDataEnabled;
}

static int PrepareTransaction(const std::string& senderAddress, const std::string& receiverAddress, const std::string& redemptionAddress,
        int64_t referenceAmount, const std::vector<unsigned char>& data, std::vector<std::pair<CScript, int64_t> >& vecSend)
{
    // Determine the class to send the transaction via - default is Class C
    int omniTxClass = OMNI_CLASS_C;
    if (!UseEncodingClassC(data.size())) omniTxClass = OMNI_CLASS_B;

    // Encode the data outputs
    switch (omniTxClass) {
        case OMNI_CLASS_B: {
            CPubKey redeemingPubKey;
            std::string str = redemptionAddress;
            if (str.empty()) str = senderAddress; // TODO: this seems laborious
            if (!AddressToPubKey(str, redeemingPubKey)) return MP_REDEMP_BAD_VALIDATION;
            if (!OmniCore_Encode_ClassB(senderAddress, redeemingPubKey, data, vecSend)) return MP_ENCODING_ERROR;
        break; }
        case OMNI_CLASS_C:
            if (!OmniCore_Encode_ClassC(data, vecSend)) return MP_ENCODING_ERROR;
        break;
    }

    // Then add a pay-to-pubkey-hash output for the recipient (if needed)
    // Note: we do this last as we want this to be the highest vout
    if (!receiverAddress.empty()) {
        CScript scriptPubKey = GetScriptForDestination(CBitcoinAddress(receiverAddress).Get());
        vecSend.push_back(std::make_pair(scriptPubKey, std::max(referenceAmount, GetDustThreshold(scriptPubKey))));
    }

    return 0;
}
// This function requests the wallet create an Omni transaction using the supplied parameters and payload
int ClassAgnosticWalletTXBuilder(const std::string& senderAddress, const std::string& receiverAddress, const std::string& redemptionAddress,
        int64_t referenceAmount, const std::vector<unsigned char>& data, uint256& txid, std::string& rawHex, bool commit)
{
    CWallet* pwallet = pwalletMain;
    if (!pwallet) { return MP_ERR_WALLET_ACCESS; }

    // Prepare the transaction - first setup some vars
    CCoinControl coinControl;
    std::vector<std::pair<CScript, int64_t> > vecSend;
    CWalletTx wtxNew;
    int64_t nFeeRet = 0;
    std::string strFailReason;
    CReserveKey reserveKey(pwallet);

    // Select the inputs
    // TODO: check, if enough coins were selected
    if (!SelectCoins(senderAddress, coinControl, referenceAmount)) { return MP_INPUTS_INVALID; }
    if (!coinControl.HasSelected()) { return MP_ERR_INPUTSELECT_FAIL; }

    // Next, we set the change address to the sender
    coinControl.destChange = CBitcoinAddress(senderAddress).Get();

    // Encode the data outputs
    int rc = PrepareTransaction(senderAddress, receiverAddress, redemptionAddress, referenceAmount, data, vecSend);
    if (rc != 0) { return rc; }

    // Ask the wallet to create the transaction (note mining fee determined by Bitcoin Core params)
    if (!pwallet->CreateTransaction(vecSend, wtxNew, reserveKey, nFeeRet, strFailReason, &coinControl)) {
        PrintToConsole("%s() ERROR: %s\n", __func__, strFailReason);
        return MP_ERR_CREATE_TX;
    }

    // If this request is only to create, but not commit the transaction then display it and exit
    if (!commit) {
        rawHex = EncodeHexTx(wtxNew);
    } else {
        // Commit the transaction to the wallet and broadcast)
        PrintToLog("%s():%s; nFeeRet = %lu, line %d, file: %s\n", __func__, wtxNew.ToString(), nFeeRet, __LINE__, __FILE__);
        if (!pwallet->CommitTransaction(wtxNew, reserveKey)) return MP_ERR_COMMIT_TX;
        txid = wtxNew.GetHash();
    }

    return 0;
}

int ClassAgnosticWalletTXBuilder(const std::vector<COutPoint>& txInputs, const std::string& receiverAddress,
        const std::vector<unsigned char>& payload, const CPubKey& pubKey, std::string& rawTxHex, int64_t txFee)
{
    int omniTxClass = OMNI_CLASS_C;
    if (!UseEncodingClassC(payload.size())) {
        omniTxClass = OMNI_CLASS_B;
    }

    PrintToConsole("%s() creating class: %d\n", __func__, omniTxClass);

    std::vector<CTxOut> txOutputs;
    if (!GetTransactionInputs(txInputs, txOutputs)) {
        PrintToConsole("%s() ERROR: failed to get transaction inputs\n", __func__);
        return false;
    }

    std::string strSender;
    int64_t amountIn = 0;
    if (!CheckTransactionInputs(txOutputs, strSender, amountIn, GetHeight())) {
        PrintToConsole("%s() ERROR: failed to check transaction inputs\n", __func__);
        return false;
    }

    std::vector<std::pair<CScript, int64_t> > vecSend;
    switch (omniTxClass) {
        case OMNI_CLASS_B:
            if (!pubKey.IsFullyValid()) {
                PrintToConsole("%s() ERROR: public key for class B redemption is invalid\n", __func__);
                return MP_REDEMP_BAD_VALIDATION;
            }
            if (!OmniCore_Encode_ClassB(strSender, pubKey, payload, vecSend)) {
                PrintToConsole("%s() ERROR: failed to embed class B payload\n", __func__);
                return MP_ENCODING_ERROR;
            }
        break;
        case OMNI_CLASS_C:
            if (!OmniCore_Encode_ClassC(payload, vecSend)) {
                PrintToConsole("%s() ERROR: failed to embed class C payload\n", __func__);
                return MP_ENCODING_ERROR;
            }
        break;
    }

    if (!receiverAddress.empty()) {
        CScript scriptPubKey = GetScriptForDestination(CBitcoinAddress(receiverAddress).Get());
        vecSend.push_back(std::make_pair(scriptPubKey, GetDustThreshold(scriptPubKey)));
    }

    CMutableTransaction rawTx;
    for (std::vector<COutPoint>::const_iterator it = txInputs.begin(); it != txInputs.end(); ++it) {
        const COutPoint& outpoint = *it;

        CTxIn input(outpoint);
        rawTx.vin.push_back(input);
    }

    int64_t amountOut = 0;
    for (std::vector<std::pair<CScript, int64_t> >::const_iterator it = vecSend.begin(); it != vecSend.end(); ++it) {
        const CScript& scriptPubKey = it->first;
        const int64_t amount = it->second;

        CTxOut out(amount, scriptPubKey);
        rawTx.vout.push_back(out);
        amountOut += amount;
    }

    int64_t amountChange = amountIn - amountOut - txFee;
    if (amountChange < 0) {
        PrintToConsole("%s() ERROR: insufficient input amount\n", __func__);
    }

    CScript scriptChange = GetScriptForDestination(CBitcoinAddress(strSender).Get());
    CTxOut rawTxOut(amountChange, scriptChange);

    if (!rawTxOut.IsDust(::minRelayTxFee)) {
        rawTx.vout.push_back(rawTxOut);
    }

    rawTxHex = EncodeHexTx(rawTx);
    PrintToConsole("%s() result: %s\n", __func__, rawTxHex);

    return 0;
}

int64_t FeeThreshold()
{
    // based on 3x <200 byte outputs (change/reference/data) & total tx size of <2KB
    return 3 * minRelayTxFee.GetFee(200) + CWallet::minTxFee.GetFee(2000);
}

int64_t FeeCheck(const std::string& address)
{
    // check the supplied address against selectCoins to determine if sufficient fees for send
    CCoinControl coinControl;
    return SelectCoins(address, coinControl, 0);
}


} // namespace mastercore
