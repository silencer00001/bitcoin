#include "omnicore/transactions.h"

#include "omnicore/errors.h"
#include "omnicore/omnicore.h"
#include "omnicore/encoding.h"
#include "omnicore/log.h"
#include "omnicore/script.h"

#include "base58.h"
#include "uint256.h"
#include "util.h"
#include "core_io.h"
#include "init.h"
#include "sync.h"
#include "pubkey.h"
#include "script/script.h"
#include "script/standard.h"
#include "primitives/transaction.h"
#include "coincontrol.h"
#include "wallet.h"

#include <stdint.h>
#include <string>
#include <utility>
#include <map>
#include <vector>

namespace mastercore
{

static int64_t SelectCoins(const std::string& fromAddress, CCoinControl& coinControl, int64_t additional) {
    CWallet* pwallet = pwalletMain;
    if (NULL == pwallet) {
        return 0;
    }

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
        bool bIsMine;
        bool bIsSpent;

        if (pcoin->IsTrusted()) {
            const int64_t nAvailable = pcoin->GetAvailableCredit();

            if (!nAvailable)
                continue;

            for (unsigned int i = 0; i < pcoin->vout.size(); i++) {
                txnouttype whichType;
                if (!GetOutputType(pcoin->vout[i].scriptPubKey, whichType))
                    continue;

                if (!IsAllowedOutputType(whichType, nHeight))
                    continue;

                CTxDestination dest;
                if (!ExtractDestination(pcoin->vout[i].scriptPubKey, dest))
                    continue;

                bIsMine = IsMine(*pwallet, dest);
                bIsSpent = pwallet->IsSpent(wtxid, i);

                if (!bIsMine || bIsSpent)
                    continue;

                int64_t n = bIsSpent ? 0 : pcoin->vout[i].nValue;

                std::string sAddress = CBitcoinAddress(dest).ToString();
                if (msc_debug_tokens)
                    PrintToLog("%s:IsMine()=%s:IsSpent()=%s:%s: i=%d, nValue= %lu\n",
                        sAddress, bIsMine ? "yes" : "NO",
                        bIsSpent ? "YES" : "no", wtxid.ToString(), i, n);

                // only use funds from the sender's address
                if (fromAddress == sAddress) {
                    COutPoint outpt(wtxid, i);
                    coinControl.Select(outpt);

                    n_total += n;

                    if (n_max <= n_total)
                        break;
                }
            }
        }

        if (n_max <= n_total)
            break;
    }

    return n_total;
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

// This function requests the wallet create an Omni transaction using the supplied parameters and payload
int ClassAgnosticWalletTXBuilder(const std::string& senderAddress, const std::string& receiverAddress, const std::string& redemptionAddress,
        int64_t referenceAmount, const std::vector<unsigned char>& data, uint256& txid, std::string& rawHex, bool commit)
{
    CWallet* pwallet = pwalletMain;
    if (!pwallet) { return MP_ERR_WALLET_ACCESS; }

    // Determine the class to send the transaction via - default is Class C
    int omniTxClass = OMNI_CLASS_C;
    if (!UseEncodingClassC(data.size())) omniTxClass = OMNI_CLASS_B;

    // Prepare the transaction - first setup some vars
    CCoinControl coinControl;
    std::vector<std::pair<CScript, int64_t> > vecSend;
    CWalletTx wtxNew;
    int64_t nFeeRet = 0;
    std::string strFailReason;
    CReserveKey reserveKey(pwallet);

    // Next, we set the change address to the sender
    coinControl.destChange = CBitcoinAddress(senderAddress).Get();

    // Select the inputs
    // TODO: check, if enough coins were selected
    if (!SelectCoins(senderAddress, coinControl, referenceAmount)) { return MP_INPUTS_INVALID; }
    if (!coinControl.HasSelected()) { return MP_ERR_INPUTSELECT_FAIL; }

    // Encode the data outputs
    switch (omniTxClass) {
        case OMNI_CLASS_B: {
            CBitcoinAddress address;
            CPubKey redeemingPubKey;
            if (!redemptionAddress.empty()) { address.SetString(redemptionAddress); } else { address.SetString(senderAddress); }
            if (address.IsValid()) { // validate the redemption address
                if (address.IsScript()) {
                    PrintToLog("%s() ERROR: Redemption Address must be specified !\n", __FUNCTION__);
                    return MP_REDEMP_ILLEGAL; // ^ misleading error (TODO fix)
                } else {
                    CKeyID keyID;
                    if (!address.GetKeyID(keyID)) return MP_REDEMP_BAD_KEYID;
                    if (!pwallet->GetPubKey(keyID, redeemingPubKey)) return MP_REDEMP_FETCH_ERR_PUBKEY;
                    if (!redeemingPubKey.IsFullyValid()) return MP_REDEMP_INVALID_PUBKEY;
                }
            } else return MP_REDEMP_BAD_VALIDATION;
            if (!OmniCore_Encode_ClassB(senderAddress, redeemingPubKey, data, vecSend)) { return MP_ENCODING_ERROR; }
        break; }
        case OMNI_CLASS_C:
            if (!OmniCore_Encode_ClassC(data, vecSend)) { return MP_ENCODING_ERROR; }
        break;
    }

    // Then add a paytopubkeyhash output for the recipient (if needed) - note we do this last as we want this to be the highest vout
    if (!receiverAddress.empty()) {
        CScript scriptPubKey = GetScriptForDestination(CBitcoinAddress(receiverAddress).Get());
        vecSend.push_back(std::make_pair(scriptPubKey, std::max(referenceAmount, GetDustThreshold(scriptPubKey))));
    }

    // Ask the wallet to create the transaction (note mining fee determined by Bitcoin Core params)
    if (!pwallet->CreateTransaction(vecSend, wtxNew, reserveKey, nFeeRet, strFailReason, &coinControl)) {
        PrintToLog("%s() ERROR: %s\n", __FUNCTION__, strFailReason);
        return MP_ERR_CREATE_TX;
    }

    // If this request is only to create, but not commit the transaction then display it and exit
    if (!commit) {
        rawHex = EncodeHexTx(wtxNew);
    } else {
        // Commit the transaction to the wallet and broadcast)
        PrintToLog("%s():%s; nFeeRet = %lu, line %d, file: %s\n", __FUNCTION__, wtxNew.ToString(), nFeeRet, __LINE__, __FILE__);
        if (!pwallet->CommitTransaction(wtxNew, reserveKey)) return MP_ERR_COMMIT_TX;
        txid = wtxNew.GetHash();
    }

    return 0;
}


} // namespace mastercore
