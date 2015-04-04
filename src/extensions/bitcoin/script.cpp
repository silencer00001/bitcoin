#include "extensions/bitcoin/script.h"

#include "amount.h"
#include "main.h"
#include "script/script.h"
#include "script/standard.h"

#include <vector>

/**
 * Determines minimum output amount to be spent by an output, based on
 * scriptPubKey size in relation to the minimum relay fee.
 *
 * @param scriptPubKey[in]  The scriptPubKey
 * @return The dust threshold value
 */
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

/**
 * Identifies standard output types based on a scriptPubKey.
 *
 * @param scriptPubKey[in]  The script
 * @return The output type
 */
txnouttype GetOutputType(const CScript& scriptPubKey) {
    std::vector<std::vector<unsigned char> > vSolutions;

    txnouttype outtype;
    if (!Solver(scriptPubKey, outtype, vSolutions)) {
        outtype = TX_NONSTANDARD;
    }

    return outtype;
}

/**
 * Extracts the pushed data from a script.
 *
 * @param script[in]      The script
 * @param vvchRet[out]    The extracted pushed data
 * @param fSkipFirst[in]  Whether the first push operation should be skipped (default: false)
 * @return True if the extraction was successful (can be empty)
 */
bool GetScriptPushes(const CScript& script, std::vector<std::vector<unsigned char> >& vvchRet, bool fSkipFirst)
{
    int count = 0;
    CScript::const_iterator pc = script.begin();
    while (pc < script.end()) {
        opcodetype opcode;
        std::vector<unsigned char> data;
        if (!script.GetOp(pc, opcode, data))
            return false;
        if (0x00 <= opcode && opcode <= OP_PUSHDATA4)
            if (count++ || !fSkipFirst) vvchRet.push_back(data);
        // TODO: check whether it makes sense to return only data with size > 0
        // TODO: check whether it makes sense to increase the range to <= OP_16
    }

    return true;
}
