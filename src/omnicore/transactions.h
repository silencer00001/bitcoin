#ifndef OMNICORE_TRANSACTIONS_H
#define OMNICORE_TRANSACTIONS_H

class uint256;
class CPubKey;
class COutPoint;

#include <stdint.h>
#include <string>
#include <vector>

namespace mastercore
{

int64_t FeeThreshold();
int64_t FeeCheck(const std::string& address);

int ClassAgnosticWalletTXBuilder(const std::string& senderAddress, const std::string& receiverAddress, const std::string& redemptionAddress,
        int64_t referenceAmount, const std::vector<unsigned char>& data, uint256& txid, std::string& rawHex, bool commit);

int ClassAgnosticWalletTXBuilder(const std::vector<COutPoint>& inputs, const std::string& receiverAddress,
        const std::vector<unsigned char>& payload, const CPubKey& pubKey, std::string& rawTxHex,
        int64_t txFee = 10000, int64_t referenceAmount = 0);

}


#endif // OMNICORE_TRANSACTIONS_H
