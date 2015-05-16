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


#endif // MASTERCORE_RPC_VALUES_H
