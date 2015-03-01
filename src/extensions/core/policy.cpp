#include "extensions/core/policy.h"

#include "extensions/bitcoin/chain.h"

#include "script/standard.h"

bool IsAllowedOutputType(txnouttype whichType, int nBlock) {
    int p2shAllowed = 0;

    if (P2SH_BLOCK <= nBlock || !IsMainNet()) {
        p2shAllowed = 1;
    }

    // Valid types:
    // 1) Pay-to-pubkey-hash
    // 2) Pay-to-script-hash (if P2SH is enabled)

    if ((TX_PUBKEYHASH == whichType) || (p2shAllowed && (TX_SCRIPTHASH == whichType))) {
        return true;
    } else {
        return false;
    }
}
