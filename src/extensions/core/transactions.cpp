#include "extensions/core/transactions.h"

#include "extensions/core/modifications/marker.h"

#include "base58.h"
#include "main.h"
#include "primitives/transaction.h"
#include "script/standard.h"

#include <stdint.h>
#include <map>
#include <string>
#include <vector>

/**
 * Checks whether a transaction is tagged with Exodus marker.
 *
 * @param tx[in]  The transaction
 * @return True if the marker was found
 */
bool HasExodusMarker(const CTransaction& tx)
{
    std::vector<CTxOut>::const_iterator it;
    for (it = tx.vout.begin(); it != tx.vout.end(); ++it) {
        CTxDestination dest;
        if (!ExtractDestination(it->scriptPubKey, dest)) {
            continue;
        }
        if (ExodusAddress().CompareTo(CBitcoinAddress(dest)) == 0) {
            return true;
        }
    }

    return false;
}

/**
 * Determines the "sender" of a transaction based on "contribution by sum".
 *
 * In case of ambiguousness, candidates are chosen based on the lexicographical
 * order of the base58 string representation (!) of the candidate.
 *
 * In practise this implies selecting the sender "by sum" via a comparison of
 * CBitcoinAddress objects would yield faulty results.
 *
 * Note: it reflects the behavior of Omni Core, but this edge case is not specified.
 *
 * @param tx[in]          The transaction to use
 * @param strSender[out]  The destination with highest output value contribution
 * @return True if it was successful
 */
bool GetSenderByContribution(const CTransaction& tx, std::string& strSender)
{
    strSender.clear();
    std::vector<CTxOut> prevOutputs;

    if (!GetTransactionInputs(tx, prevOutputs)) {
        return false;
    }

    return GetSenderByContribution(prevOutputs, strSender);
}

/**
 * Determines the "sender" based on "contribution by sum" of output values.
 *
 * @param vouts[in]       The outputs to use
 * @param strSender[out]  The destination with highest output value contribution
 * @return True if it was successful
 */
bool GetSenderByContribution(const std::vector<CTxOut>& vouts, std::string& strSender)
{
    strSender.clear();
    std::map<std::string, int64_t> mapInputSum;

    for (unsigned int n = 0; n < vouts.size(); n++) {
        CTxDestination dest;
        if (!ExtractDestination(vouts[n].scriptPubKey, dest)) {
            return false;
        }
        CBitcoinAddress addressSource(dest);
        mapInputSum[addressSource.ToString()] += vouts[n].nValue;
    }

    int64_t nMax = 0;
    std::map<std::string, int64_t>::const_iterator it;
    for (it = mapInputSum.begin(); it != mapInputSum.end(); ++it) {
        int64_t nTemp = it->second;
        if (nTemp > nMax) {
            strSender = it->first;
            nMax = nTemp;
        }
    }

    return !strSender.empty();
}

/**
 * Retrieves the outputs of all inputs of a transaction.
 *
 * @param tx[in]         The transaction to use
 * @param voutsRet[out]  The outputs matching the inputs of the transaction
 * @return True if it was successful
 */
bool GetTransactionInputs(const CTransaction& tx, std::vector<CTxOut>& voutsRet)
{
    voutsRet.clear();
    voutsRet.reserve(tx.vin.size());

    // Gather outputs for all transaction inputs  
    for (std::vector<CTxIn>::const_iterator it = tx.vin.begin(); it != tx.vin.end(); ++it) {
        const COutPoint& outpoint = it->prevout;

        CTransaction prevTx;
        uint256 prevTxBlock;
        if (!GetTransaction(outpoint.hash, prevTx, prevTxBlock, false)) {
            return false;
        }
        // Sanity check
        if (0 < outpoint.n || prevTx.vout.size() < outpoint.n) {
            return false;
        }
        CTxOut prevTxOut = prevTx.vout[outpoint.n];
        voutsRet.push_back(prevTxOut);
    }

    // Sanity check
    return (tx.vin.size() == voutsRet.size());
}

// TODO: remove script/standard dependency
// TODO: maybe move GetTransactionInputs() into a less specific section
// TODO: maybe group all requirements, such as HasExodusMarker(), somewhere else
// TODO: generalize, be able to work with several different selections
