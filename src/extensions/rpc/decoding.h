#ifndef EXTENSIONS_RPC_DECODING_H
#define EXTENSIONS_RPC_DECODING_H

#include "json/json_spirit_value.h"

json_spirit::Value decode_payload(const json_spirit::Array& params, bool fHelp);
json_spirit::Value get_payload(const json_spirit::Array& params, bool fHelp);

#endif // EXTENSIONS_RPC_DECODING_H
