// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_RPCCLIENT_H
#define BITCOIN_RPCCLIENT_H

#include "json/json_spirit_reader_template.h"
#include "json/json_spirit_utils.h"
#include "json/json_spirit_writer_template.h"

#include <set>
#include <string>
#include <utility>

class CRPCConvertTable
{
private:
    std::set<std::pair<std::string, int> > members;

public:
    CRPCConvertTable();

    /**
     * Adds new conversion to the conversion table
     * @param method name of the method
     * @param idx    0-based index of the paramter
     */
    void addConversion(const std::string& method, int idx);

    /**
     * Determines if a paramter should be handled as quoted or unquoted string
     * @param method name of the method
     * @param idx    0-based index of the paramter
     * @return whether the paramter should be converted
     */
    bool convert(const std::string& method, int idx);
};

extern CRPCConvertTable rpcCvtTable;

json_spirit::Array RPCConvertValues(const std::string& strMethod, const std::vector<std::string>& strParams);

#endif // BITCOIN_RPCCLIENT_H
