#include "extensions/primitives/transaction.h"

#include "uint256.h"
#include "utilstrencodings.h"

#include <stdint.h>
#include <cstring>
#include <string>

CMPTransaction::CMPTransaction()
{
    SetNull();
}

void CMPTransaction::SetNull()
{
    txid = 0;
    tx_idx = 0;
    nValue = 0;
    nNewValue = 0;
    tx_fee_paid = 0;
    block = -1;
    pkt_size = 0;
    sender.erase();
    receiver.erase();
    blockTime = 0;
    memset(&pkt, 0, sizeof (pkt));
}

void CMPTransaction::Set(const uint256& t, int b, unsigned int idx, int64_t bt)
{
    txid = t;
    block = b;
    tx_idx = idx;
    blockTime = bt;
}

void CMPTransaction::Set(const std::string& s, const std::string& r, uint64_t n, const uint256& t, int b, unsigned int idx, unsigned char* p, unsigned int size, int fMultisig, uint64_t txf)
{
    sender = s;
    receiver = r;
    txid = t;
    block = b;
    tx_idx = idx;
    pkt_size = size < sizeof (pkt) ? size : sizeof (pkt);
    nValue = n;
    nNewValue = n;
    multi = fMultisig;
    tx_fee_paid = txf;
    memcpy(&pkt, p, pkt_size);
}

std::string CMPTransaction::getDecodedPayload() const
{
   std::vector<unsigned char> sec;
   sec.resize(pkt_size);
   memcpy(&sec[0], &pkt[0], pkt_size);
   return HexStr(sec);
}
