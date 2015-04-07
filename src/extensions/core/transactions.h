#ifndef EXTENSIONS_CORE_TRANSACTIONS_H
#define EXTENSIONS_CORE_TRANSACTIONS_H

class CTransaction;
class CTxOut;

#include <vector>
#include <string>

/** Checks whether a transaction is tagged with Exodus marker. */
bool HasExodusMarker(const CTransaction& tx);
/** Determines the "sender" of a meta transaction based on "contribution by sum". */
bool GetSenderByContribution(const CTransaction& tx, std::string& strSenderRet);
/** Determines the "sender" based on "contribution by sum" of output values. */
bool GetSenderByContribution(const std::vector<CTxOut>& vouts, std::string& strSenderRet);
/** Retrieves the outputs of all inputs of a transaction. */
bool GetTransactionInputs(const CTransaction& tx, std::vector<CTxOut>& voutsRet);

#endif // EXTENSIONS_CORE_TRANSACTIONS_H
