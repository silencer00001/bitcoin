#include "extensions/core/modifications/marker.h"

#include "base58.h"
#include "chainparams.h"

#include <string>

const CBitcoinAddress ExodusAddress()
{
    return ExodusAddress(Params().NetworkIDString());
}

const CBitcoinAddress ExodusAddress(const std::string& strNetworkId)
{
    if (strNetworkId == "main" || strNetworkId == "unittest") {
        return CBitcoinAddress("1EXoDusjGwvnjZUyKkxZ4UHEf77z6A5S4P");
    } else {
        return CBitcoinAddress("mpexoDuSkGGqvqrkrjiFng38QPkJQVFyqv");
    }
}
