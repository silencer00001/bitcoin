#ifndef EXTENSIONS_BITCOIN_SCRIPT_H
#define EXTENSIONS_BITCOIN_SCRIPT_H

#include "script/standard.h"

#include <stdint.h>
#include <string>
#include <vector>

class CScript;

/** Determines minimum output amount to be spent by an output. */
int64_t GetDustThreshold(const CScript& scriptPubKey);
/** Identifies standard output types based on a scriptPubKey. */
txnouttype GetOutputType(const CScript& scriptPubKey);
/** Extracts the pushed data from a script. */
bool GetScriptPushes(const CScript& script, std::vector<std::vector<unsigned char> >& vvchRet, bool fSkipFirst = false);

#endif // EXTENSIONS_BITCOIN_SCRIPT_H
