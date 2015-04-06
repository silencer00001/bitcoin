#ifndef EXTENSIONS_CORE_NETWORK_H
#define EXTENSIONS_CORE_NETWORK_H

#include "base58.h"

struct CEnvironmentBase
{
    CBitcoinAddress ExodusAddress;
    CBitcoinAddress MoneyManAddress;
};

struct CEnvironmentMain
{
    static const CBitcoinAddress ExodusAddress("1EXoDusjGwvnjZUyKkxZ4UHEf77z6A5S4P");
    static const CBitcoinAddress MoneyManAddress("1EXoDusjGwvnjZUyKkxZ4UHEf77z6A5S4P");
};

struct CEnvironmentTest
{
    static const CBitcoinAddress ExodusAddress("mpexoDuSkGGqvqrkrjiFng38QPkJQVFyqv");
    static const CBitcoinAddress MoneyManAddress("moneyqMan7uh8FqdCA2BV5yZ8qVrc9ikLP");
};

#endif // EXTENSIONS_CORE_NETWORK_H
