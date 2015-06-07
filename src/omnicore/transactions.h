#ifndef OMNICORE_TRANSACTIONS_H
#define OMNICORE_TRANSACTIONS_H

class uint256;

#include <stdint.h>
#include <string>
#include <vector>

namespace mastercore
{

int64_t FeeThreshold();
int64_t FeeCheck(const std::string& address);

int ClassAgnosticWalletTXBuilder(const std::string& senderAddress, const std::string& receiverAddress, const std::string& redemptionAddress,
        int64_t referenceAmount, const std::vector<unsigned char>& data, uint256& txid, std::string& rawHex, bool commit);



}




#endif // OMNICORE_TRANSACTIONS_H
