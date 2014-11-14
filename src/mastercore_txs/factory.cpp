#include "mastercore_txs/factory.h"

#include "mastercore_txs/transactions.h"

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>

namespace mastercore {
namespace transaction {

bool tryDeserializeTransaction(const std::vector<unsigned char>& vch, CTemplateBase& retTx)
{
    return retTx.Deserialize(vch);
}

CTemplateBase* tryCreateTransaction(const std::vector<unsigned char>& vch)
{
    CTemplateTypePrefix txPrefix;

    if (!txPrefix.Deserialize(vch)) {
        return NULL;
    }

    if (txPrefix.version == 0 && txPrefix.type == 0) {
        return new CTemplateSimpleSend();
    }

    if (txPrefix.version == 0 && txPrefix.type == 3) {
        return new CTemplateSendToOwners();
    }

    if (txPrefix.version == 0 && txPrefix.type == 20) {
        return new CTemplateOfferTokensV0();
    }

    if (txPrefix.version == 1 && txPrefix.type == 20) {
        return new CTemplateOfferTokensV1();
    }

    if (txPrefix.version == 0 && txPrefix.type == 21) {
        return new CTemplateTradeTokens();
    }

    if (txPrefix.version == 0 && txPrefix.type == 22) {
        return new CTemplateAcceptOffer();
    }

    if (txPrefix.version == 0 && txPrefix.type == 50) {
        return new CTemplateCreateProperty();
    }

    if (txPrefix.version == 0 && txPrefix.type == 51) {
        return new CTemplateCreateCrowdsale();
    }

    if (txPrefix.version == 1 && txPrefix.type == 51) {
        return new CTemplateCreateCrowdsale();
    }

    if (txPrefix.version == 0 && txPrefix.type == 53) {
        return new CTemplateCloseCrowdsale();
    }

    if (txPrefix.version == 0 && txPrefix.type == 54) {
        return new CTemplateCreateManagedProperty();
    }

    if (txPrefix.version == 0 && txPrefix.type == 55) {
        return new CTemplateGrantToken();
    }

    if (txPrefix.version == 0 && txPrefix.type == 56) {
        return new CTemplateRevokeToken();
    }

    if (txPrefix.version == 0 && txPrefix.type == 70) {
        return new CTemplateChangeIssuer();
    }

    return new CTemplateTypePrefix();
}

CTemplateBase* tryDeserializeTransaction(const std::vector<unsigned char>& vch)
{
    CTemplateBase* tx = tryCreateTransaction(vch);

    if (!tx->Deserialize(vch)) {
        delete tx;
        tx = NULL;
    }

    return tx;
}

} // namespace transaction
} // namespace mastercore
