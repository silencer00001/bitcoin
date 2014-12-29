#ifndef MASTERCOIN_SCRIPT_H
#define MASTERCOIN_SCRIPT_H

#include <string>
#include <vector>

class CScript;

bool GetScriptPushes(const CScript& scriptIn, std::vector<std::string>& vstrRet, bool fNoSkipFirst = true);

#endif // MASTERCOIN_SCRIPT_H
