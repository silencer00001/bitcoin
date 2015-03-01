#ifndef EXTENSIONS_CORE_DECODING_H
#define EXTENSIONS_CORE_DECODING_H

class CTransaction;
class CMPTransaction;

int parseTransaction(bool bRPConly, const CTransaction& tx, int nBlock, unsigned int idx, CMPTransaction* pMetaTx, unsigned int nTime = 0);

#endif // EXTENSIONS_CORE_DECODING_H
