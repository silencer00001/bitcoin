#ifndef MASTERCORE_RPC_VALUES_H
#define MASTERCORE_RPC_VALUES_H

#include "json/json_spirit_value.h"

#include <string>

std::string ParseAddress(const json_spirit::Value& value);

#endif // MASTERCORE_RPC_VALUES_H
