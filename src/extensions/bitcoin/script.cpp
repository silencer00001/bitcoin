#include "extensions/bitcoin/script.h"

#include "amount.h"
#include "main.h"
#include "script/script.h"
#include "script/standard.h"
#include "utilstrencodings.h"

#include <string>
#include <vector>

///
/// Determines minimum output amount to be spent by an output based on
/// scriptPubKey size in relation to the minimum relay fee.
///
int64_t GetDustThreshold(const CScript& scriptPubKey)
{
    // The total size is based on a typical scriptSig size of 148 byte,
    // 8 byte accounted for the size of output value and the serialized
    // size of scriptPubKey.
    size_t nSize = GetSerializeSize(scriptPubKey, SER_DISK, 0) + 156u;

    // The minimum relay fee dictates a threshold value under which a
    // transaction won't be relayed.
    int64_t nRelayTxFee = minRelayTxFee.GetFee(nSize);

    // A transaction is considered as "dust", if less than 1/3 of the
    // minimum fee required to relay a transaction is spent by one of
    // it's outputs. The minimum relay fee is defined per 1000 byte.
    return nRelayTxFee * 3;
}

bool GetOutputType(const CScript& scriptPubKey, txnouttype& typeRet) {
    std::vector<std::vector<unsigned char> > vSolutions;

    if (!Solver(scriptPubKey, typeRet, vSolutions))
        return false;

    return true;
}

bool GetScriptPushes(const CScript& scriptIn, std::vector<std::string>& vstrRet, bool fSkipFirst)
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
            if (count++ || !fSkipFirst) vstrRet.push_back(HexStr(data));
    }

    return true;
}
