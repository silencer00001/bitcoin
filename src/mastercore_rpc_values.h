#ifndef MASTERCORE_RPC_VALUES_H
#define MASTERCORE_RPC_VALUES_H

#include "mastercore_sp.h"

#include "json/json_spirit_value.h"

#include <stdint.h>
#include <string>

std::string ParseAddress(const json_spirit::Value& value);
uint32_t ParsePropertyIdUnchecked(const json_spirit::Value& value);
uint32_t ParsePropertyId(const json_spirit::Value& value, CMPSPInfo::Entry& info);
uint32_t ParsePropertyId(const json_spirit::Value& value);
int64_t ParseAmount(const json_spirit::Value& value, bool fDivisible);
int64_t ParseAmount(const json_spirit::Value& value, int propertyType);
uint8_t ParseEcosystem(const json_spirit::Value& value);
uint16_t ParsePropertyType(const json_spirit::Value& value);
uint32_t ParsePreviousPropertyId(const json_spirit::Value& value);
std::string ParseText(const json_spirit::Value& value);
uint8_t ParsePaymentTimeframe(const json_spirit::Value& value);
int64_t ParseDeadline(const json_spirit::Value& value);
uint8_t ParseDexAction(const json_spirit::Value& value);
uint8_t ParseMetaDexAction(const json_spirit::Value& value);
int64_t ParseCommitmentFee(const json_spirit::Value& value);
uint8_t ParseEarlyBirdBonus(const json_spirit::Value& value);
uint8_t ParseIssuerBonus(const json_spirit::Value& value);


#endif // MASTERCORE_RPC_VALUES_H
