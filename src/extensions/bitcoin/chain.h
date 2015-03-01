#ifndef EXTENSIONS_BITCOIN_CHAIN_H
#define EXTENSIONS_BITCOIN_CHAIN_H

#include "chainparams.h"

inline bool IsMainNet()
{
    return Params().NetworkIDString() == "main";
}

inline bool IsTestNet()
{
    return Params().NetworkIDString() == "test";
}

inline bool IsRegTest()
{
    return Params().NetworkIDString() == "regtest";
}

#endif // EXTENSIONS_BITCOIN_CHAIN_H
