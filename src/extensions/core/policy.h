#ifndef EXTENSIONS_CORE_POLICY_H
#define EXTENSIONS_CORE_POLICY_H

#include "script/standard.h"

#define PACKET_SIZE_CLASS_A           19
#define MAX_BTC_OUTPUTS               16

enum BLOCKHEIGHTRESTRICTIONS {
    START_REGTEST_BLOCK         =      5,
    MONEYMAN_REGTEST_BLOCK      =    101,
    GENESIS_BLOCK               = 249498,
    LAST_EXODUS_BLOCK           = 255365,
    POST_EXODUS_BLOCK           = 255366,
    START_TESTNET_BLOCK         = 263000,
    MONEYMAN_TESTNET_BLOCK      = 270775,
    MSC_DEX_BLOCK               = 290630,
    MSC_SP_BLOCK                = 297110,
    P2SH_BLOCK                  = 322000,
    MSC_MANUALSP_BLOCK          = 323230,
    MSC_STO_BLOCK               = 342650,
    MSC_METADEX_BLOCK           = 999999,
    MSC_BET_BLOCK               = 999999
};

bool IsAllowedOutputType(txnouttype whichType, int nBlock);

#endif // EXTENSIONS_CORE_POLICY_H
