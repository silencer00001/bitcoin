#include "mastercore_script.h"

#include <string>
#include <vector>

#include "script.h"
#include "util.h"

bool GetScriptPushes(const CScript& scriptIn, std::vector<std::string>& vstrRet, bool fNoSkipFirst)
{
    int count = 0;
    CScript::const_iterator pc = scriptIn.begin();
    while (pc < scriptIn.end())
    {
        opcodetype opcode;
        std::vector<unsigned char> data;
        if (!scriptIn.GetOp(pc, opcode, data))
            return false;
        if (0 <= opcode && opcode <= OP_PUSHDATA4)
            if (count++ || fNoSkipFirst) vstrRet.push_back(HexStr(data));
    }
    return true;
}
