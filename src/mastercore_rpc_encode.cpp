#include "mastercore_rpc_extensions.h"

#include "mastercore_txs/transactions.h"
#include "mastercore_types.h"

#include "rpcprotocol.h"
#include "rpcserver.h"
#include "util.h"

#include "json/json_spirit_value.h"

#include <stdexcept>
#include <string>
#include <vector>

using json_spirit::Array;
using json_spirit::Value;
using std::runtime_error;

using namespace mastercore::transaction;
using namespace mastercore::types;

static std::string EncodeAsHex(const CTemplateBase& tx)
{
    std::vector<unsigned char> vch = tx.Serialize();

    return HexStr(vch);
}

Value encode_simple_send(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_simple_send property amount\n"
    );

    CTemplateSimpleSend tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]));

    return EncodeAsHex(tx);
}

Value encode_send_to_owners(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_send_to_owners property amount\n"
    );

    CTemplateSendToOwners tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]));

    return EncodeAsHex(tx);
}

Value encode_offer_tokens(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 6)
        throw runtime_error(
            "encode_offer_tokens property amount_for_sale amount_desired "
            "block_time_limit commitment_fee subaction\n"
    );

    CTemplateOfferTokensV1 tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]),
            Cast<TokenAmountType>(params[2]),
            Cast<BlockIntervalType>(params[3]),
            Cast<TokenAmountType>(params[4]),
            Cast<DexActionType>(params[5]));

    return EncodeAsHex(tx);
}

Value encode_trade_tokens(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 5)
        throw runtime_error(
            "encode_trade_tokens property amount_for_sale property_desired "
            "amount_desired subaction\n"
    );

    CTemplateTradeTokens tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]),
            Cast<PropertyIdentifierType>(params[2]),
            Cast<TokenAmountType>(params[3]),
            Cast<MetaDexActionType>(params[4]));

    return EncodeAsHex(tx);
}

Value encode_accept_offer(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_accept_offer property amount\n"
    );

    CTemplateAcceptOffer tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]));

    return EncodeAsHex(tx);
}

Value encode_create_property(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 9)
        throw runtime_error(
            "encode_create_property ecosystem property_type previous_property_id "
            "category subcategory name url data amount\n"
    );

    CTemplateCreateProperty tx(
            Cast<EcosystemType>(params[0]),
            Cast<PropertyType>(params[1]),
            Cast<PropertyIdentifierType>(params[2]),
            Cast<StringType>(params[3]),
            Cast<StringType>(params[4]),
            Cast<StringType>(params[5]),
            Cast<StringType>(params[6]),
            Cast<StringType>(params[7]),
            Cast<TokenAmountType>(params[8]));

    return EncodeAsHex(tx);
}

Value encode_create_crowdsale(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 13)
        throw runtime_error(
            "encode_create_crowdsale ecosystem property_type previous_property_id "
            "category subcategory name url data property_desired token_per_unit_vested"
            "deadline early_bird_bonus issuer_bonus\n"
    );

    CTemplateCreateCrowdsale tx(
            Cast<EcosystemType>(params[0]),
            Cast<PropertyType>(params[1]),
            Cast<PropertyIdentifierType>(params[2]),
            Cast<StringType>(params[3]),
            Cast<StringType>(params[4]),
            Cast<StringType>(params[5]),
            Cast<StringType>(params[6]),
            Cast<StringType>(params[7]),
            Cast<PropertyIdentifierType>(params[8]),            
            Cast<TokenAmountType>(params[9]),
            Cast<TimestampType>(params[10]),
            Cast<PercentageType>(params[11]),
            Cast<PercentageType>(params[12]));

    return EncodeAsHex(tx);
}

Value encode_close_crowdsale(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "encode_close_crowdsale property\n"
    );

    CTemplateCloseCrowdsale tx(
            Cast<PropertyIdentifierType>(params[0]));

    return EncodeAsHex(tx);
}

Value encode_create_managed_property(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 9)
        throw runtime_error(
            "encode_create_managed_property ecosystem property_type "
            "previous_property_id category subcategory name url data\n"
    );

    CTemplateCreateManagedProperty tx(
            Cast<EcosystemType>(params[0]),
            Cast<PropertyType>(params[1]),
            Cast<PropertyIdentifierType>(params[2]),
            Cast<StringType>(params[3]),
            Cast<StringType>(params[4]),
            Cast<StringType>(params[5]),
            Cast<StringType>(params[6]),
            Cast<StringType>(params[7]));

    return EncodeAsHex(tx);
}

Value encode_grant_token(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_grant_token property amount\n"
    );

    CTemplateGrantToken tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]));

    return EncodeAsHex(tx);
}

Value encode_revoke_token(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "encode_revoke_token property amount\n"
    );

    CTemplateRevokeToken tx(
            Cast<PropertyIdentifierType>(params[0]),
            Cast<TokenAmountType>(params[1]));

    return EncodeAsHex(tx);
}

Value encode_change_issuer(const Array& params, bool fHelp) {
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "encode_change_issuer property\n"
    );

    CTemplateChangeIssuer tx(
            Cast<PropertyIdentifierType>(params[0]));

    return EncodeAsHex(tx);
}
