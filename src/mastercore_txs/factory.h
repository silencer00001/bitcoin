#ifndef MASTERCORE_TRANSACTIONS_FACTORY_H
#define MASTERCORE_TRANSACTIONS_FACTORY_H

#include <vector>

namespace mastercore {
namespace transaction {
    
class CTemplateBase;

bool tryDeserializeTransaction(const std::vector<unsigned char>& vch, CTemplateBase& retTx);

CTemplateBase* tryCreateTransaction(const std::vector<unsigned char>& vch);

CTemplateBase* tryDeserializeTransaction(const std::vector<unsigned char>& vch);

} // namespace transaction
} // namespace mastercore

#endif // MASTERCORE_TRANSACTIONS_FACTORY_H
