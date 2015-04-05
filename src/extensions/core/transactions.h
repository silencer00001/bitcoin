#ifndef EXTENSIONS_CORE_TRANSACTIONS_H
#define EXTENSIONS_CORE_TRANSACTIONS_H

class CTransaction;
class CTxOut;

#include <vector>
#include <string>

bool HasExodusMarker(const CTransaction& tx);
bool GetSenderByContribution(const std::vector<CTxOut>& vouts, std::string& strSender);

#endif // EXTENSIONS_CORE_TRANSACTIONS_H
