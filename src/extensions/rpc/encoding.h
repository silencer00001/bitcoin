#ifndef EXTENSIONS_RPC_ENCODING_H
#define EXTENSIONS_RPC_ENCODING_H

#include "json/json_spirit_value.h"

json_spirit::Value null_data(const json_spirit::Array& params, bool fHelp);
json_spirit::Value bare_multisig(const json_spirit::Array& params, bool fHelp);
json_spirit::Value obfuscated_multisig(const json_spirit::Array& params, bool fHelp);

#endif // EXTENSIONS_RPC_ENCODING_H
