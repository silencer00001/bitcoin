#ifndef EXTENSIONS_PRIMITIVES_TRANSACTION_H
#define EXTENSIONS_PRIMITIVES_TRANSACTION_H

#include "uint256.h"

#include <stdint.h>
#include <string>

#define PACKET_SIZE         31
#define MAX_PACKETS         64

class CMPTransaction
{
public:
    CMPTransaction();

    void SetNull();
    void Set(const uint256& t, int b, unsigned int idx, int64_t bt);
    void Set(const std::string& s, const std::string& r, uint64_t n, const uint256& t, int b, unsigned int idx, unsigned char* p, unsigned int size, int fMultisig, uint64_t txf);
    
    std::string getDecodedPayload() const;

private:
    std::string sender;
    std::string receiver;
    uint256 txid;
    int block;
    unsigned int tx_idx;
    int pkt_size;
    unsigned char pkt[1 + MAX_PACKETS * PACKET_SIZE];
    uint64_t nValue;
    uint64_t nNewValue;
    int multi;
    uint64_t tx_fee_paid;
    int64_t blockTime;
};

#endif // EXTENSIONS_PRIMITIVES_TRANSACTION_H
