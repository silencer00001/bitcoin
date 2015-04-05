#include "extensions/core/transactions.h"

#include "extensions/core/modifications/marker.h"

#include "base58.h"
#include "script/standard.h"
#include "primitives/transaction.h"

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <map>
#include <string>

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

bool GetSenderByContribution(const std::vector<CTxOut>& vouts, std::string& strSender)
{
    strSender.clear();
    std::map<std::string, uint64_t> mapInputSum;

    for (unsigned int n = 0; n < vouts.size(); n++) {
        CTxDestination dest;
        if (!ExtractDestination(vouts[n].scriptPubKey, dest)) {
            continue;
        }

        CBitcoinAddress addressSource(dest);
        mapInputSum[addressSource.ToString()] += vouts[n].nValue;
    }

    uint64_t nMax = 0;
    std::map<std::string, uint64_t>::const_iterator it;
    for (it = mapInputSum.begin(); it != mapInputSum.end(); ++it) {
        
        printf("%s = %lu\n", it->first.c_str(), it->second);

        uint64_t nTemp = it->second;
        if (nTemp > nMax) {
            strSender = it->first;
            nMax = nTemp;
        }
    }

    return !strSender.empty();
}
