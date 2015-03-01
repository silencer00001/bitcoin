#ifndef EXTENSIONS_BITCOIN_SCRIPT_H
#define EXTENSIONS_BITCOIN_SCRIPT_H

#include "script/standard.h"

#include <string>
#include <vector>

class CScript;

int64_t GetDustThreshold(const CScript& scriptPubKey);
bool GetOutputType(const CScript& scriptPubKey, txnouttype& typeRet);
bool GetScriptPushes(const CScript& scriptIn, std::vector<std::string>& vstrRet, bool fSkipFirst = false);

#endif // EXTENSIONS_BITCOIN_SCRIPT_H
