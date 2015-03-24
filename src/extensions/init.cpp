#include "extensions/init.h"

#include "extensions/rpc/decoding.h"
#include "extensions/rpc/encoding.h"

#include "rpcserver.h"

static const CRPCCommand vRPCCommands[] =
{ //  category       name                    actor (function)       okSafeMode threadSafe reqWallet
  //  -------------- ----------------------- ---------------------- ---------- ---------- ---------
    {"hidden",       "get_payload",          &get_payload,          false,     false,     false},
    {"hidden",       "decode_payload",       &decode_payload,       true,      true,      false},

    {"encoding",     "null_data",            &null_data,            true,      true,      false},
    {"encoding",     "bare_multisig",        &bare_multisig,        true,      true,      false},
    {"encoding",     "obfuscated_multisig",  &obfuscated_multisig,  true,      true,      false},    
};

static void AddRPCCommands()
{
    size_t nSize = sizeof(vRPCCommands) / sizeof(vRPCCommands[0]);

    for (size_t n = 0; n < nSize; ++n) {
        const CRPCCommand* pcommand = &vRPCCommands[n];
        tableRPC.AddOrReplaceCommand(pcommand);
    }
}

void Extensions::Initialize()
{
    AddRPCCommands();
}
