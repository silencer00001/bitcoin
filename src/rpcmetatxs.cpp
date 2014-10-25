#include "core.h"
#include "key.h"
#include "main.h"
#include "rpcserver.h"
#include "script.h"
#include "util.h"

#include <stdint.h>
#include <string.h>
#include <vector>

#include "json/json_spirit_utils.h"
#include "json/json_spirit_value.h"
#include "base58.h"

using namespace json_spirit;

#define _DATA_ARRAY_SIZE 17
#define _DUMP_LINE_EVERY_N_BLOCKS 2500

enum metatype
{
    META_SATOSHI_DICE = 4,
    META_BETCOIN_DICE = 5,
    META_SATOSHI_BONES = 6,
    META_LUCKY_BIT = 7,
    META_COUNTERPARTY = 8,
    META_MASTERCOIN = 9,
    META_CHANCECOIN = 10,
    META_PROOF_OF_EXISTENCE = 11,
    META_OPEN_ASSETS = 12,
    META_BLOCK_SIGN = 13,
    META_COIN_SPARK = 14,
    META_CRYPTO_TESTS = 15,
    META_OTHER = 16
};

static const char* GetMetaType(metatype t)
{
    switch (t)
    {
        case META_SATOSHI_DICE: return "SatoshiDice";
        case META_BETCOIN_DICE: return "BetCoinDice";
        case META_SATOSHI_BONES: return "SatoshiBones";
        case META_LUCKY_BIT: return "LuckyBit";
        case META_COUNTERPARTY: return "Counterparty";
        case META_MASTERCOIN: return "Mastercoin";
        case META_CHANCECOIN: return "Chancecoin";
        case META_PROOF_OF_EXISTENCE: return "ProofOfExistence";
        case META_OPEN_ASSETS: return "OpenAssets";
        case META_BLOCK_SIGN: return "BlockSign";
        case META_COIN_SPARK: return "CoinSpark";
        case META_CRYPTO_TESTS: return "CryptoTests";
        case META_OTHER: return "Other";
    }

    return NULL;
};

static int MetaTypeToInt(metatype t)
{
    return static_cast<int>(t);
};

static metatype IntToMetaType(int i)
{
    return static_cast<metatype>(i);
};

struct MetaPrefixRange {
    uint32_t begin;
    uint32_t end;
    metatype name;
};

struct MetaPrefix {
    std::vector<unsigned char> prefix;
    metatype name;
};

static struct MetaPrefixRange PubKeyHashPrefixes[] = {
    {0x06f1b600, 0x06f1b6ff, META_SATOSHI_DICE},              // SatoshiDice
    {0x74db3700, 0x74db59ff, META_BETCOIN_DICE},              // BetCoin Dice
    {0x069532d8, 0x069532da, META_SATOSHI_BONES},             // SatoshiBone
    {0xda5dde84, 0xda5dde94, META_LUCKY_BIT},                 // Lucky Bit
    {0x434e5452, 0x434e5452, META_COUNTERPARTY},              // Counterparty
    {0x946cb2e0, 0x946cb2e0, META_MASTERCOIN},                // Mastercoin, 1EXoDusjGwvnjZUyKkxZ4UHEf77z6A5S4P
    //{0x643ce12b, 0x643ce12b, META_MASTERCOIN},              // Mastercoin, mpexoDuSkGGqvqrkrjiFng38QPkJQVFyqv
    //{0xc4c5d791, 0xc4c5d791, META_CHBS},                    // CHBS, 1JwSSubhmg6iPtRjtyqhUYYH7bZg3Lfy1T
};

static struct MetaPrefix MultisigPrefixes[] = {
    {ParseHex("434e545250525459"), META_COUNTERPARTY},        // "CNTRPRTY", Counterparty
    {ParseHex("4348414e4345434f"), META_CHANCECOIN},          // "CHANCECO", Chancecoin    
    //{ParseHex("747261646c65"),     META_TRADLE}             // "tradle", https://github.com/urbien/tradle/blob/master/README.md
};

static struct MetaPrefix OpReturnPrefixes[] = {    
    {ParseHex("434e545250525459"), META_COUNTERPARTY},        // "CNTRPRTY", Counterparty
    {ParseHex("444f4350524f4f46"), META_PROOF_OF_EXISTENCE},  // "DOCPROOF", Proof of Existence
    {ParseHex("4f41"),             META_OPEN_ASSETS},         // "OA", Open Assets    
    {ParseHex("4253"),             META_BLOCK_SIGN},          // "BS", BlockSign    
    {ParseHex("53504b74"),         META_COIN_SPARK},          // "SPKt", CoinSpark
    {ParseHex("43727970746f5465737473"), META_CRYPTO_TESTS},  // "CryptoTests", http://crypto-copyright.com/   
    //{ParseHex("42495450524f4f46"), META_BITPROOF},          // "BITPROOF", bitproof.io
    //{ParseHex("747261646c65"),     META_TRADLE},            // "tradle", https://github.com/urbien/tradle/blob/master/README.md
    //{ParseHex("534d415254415353"), META_SMARTASS},          // "SMARTASS"
    //{ParseHex("466163746f6d"),     META_FACTOM},            // "Factom"
    //{ParseHex("00756e7375636365"), META_UNSUCCESSFUL_DOUBLE_SPENT},  // " unsuccessful double-spend attempt ", Peter Todd's unsuccessful double spent
};

// OP_DUP OP_HASH160 [payload] OP_EQUALVERIFY OP_CHECKSIG
static bool IsLoadedPubKeyHash(const CScript& scriptPubKey, txnouttype& typeRet, metatype& metaRet)
{
    if (scriptPubKey.size() < 7 || scriptPubKey[0] != OP_DUP) {
        return false;
    }

    uint32_t pfx = ntohl(*(uint32_t*)&scriptPubKey[3]);
    
    for (size_t i = 0; i < (sizeof(PubKeyHashPrefixes) / sizeof(PubKeyHashPrefixes[0])); ++i)
    {
        if (pfx >= PubKeyHashPrefixes[i].begin && pfx <= PubKeyHashPrefixes[i].end)
        {
            typeRet = TX_PUBKEYHASH;
            metaRet = PubKeyHashPrefixes[i].name;
            return true;
        }
    }

    return false;
};

// OP_n [payload] ([payload]) ([payload]) OP_m OP_CHECKMULTISIG
static bool IsLoadedMultisig(const CScript& scriptPubKey, txnouttype& typeRet, metatype& metaRet)
{
    if (scriptPubKey.size() < 36 || scriptPubKey.back() != OP_CHECKMULTISIG) {
        return false;
    }
    
    // Find position of data package
    // Assume first key is used for redemption, second carries data
    size_t offset = 0;
    
    // Compressed or uncompressed first public key?
    if (0x21 == scriptPubKey[1] && 0x21 == scriptPubKey[35]) {
        offset = 36;
    }
    else
    if (scriptPubKey.size() >= 68 && 0x41 == scriptPubKey[1] && 0x21 == scriptPubKey[67]) {
        offset = 68;
    }

    for (size_t i = 0; i < (sizeof(MultisigPrefixes) / sizeof(MultisigPrefixes[0])); ++i)
    {
        // The data package might be prefixed
        if ((0 == memcmp(&(MultisigPrefixes[i].prefix)[0],
                &scriptPubKey[offset+1], MultisigPrefixes[i].prefix.size())) ||
            (0 == memcmp(&(MultisigPrefixes[i].prefix)[0],
                &scriptPubKey[offset], MultisigPrefixes[i].prefix.size())))
        {
            typeRet = TX_MULTISIG;
            metaRet = MultisigPrefixes[i].name;
            return true;
        }
    }

    return false;
};

// OP_RETURN [payload]
static bool IsLoadedOpReturn(const CScript& scriptPubKey, txnouttype& typeRet, metatype& metaRet)
{
    if (scriptPubKey.size() < 3 || scriptPubKey[0] != OP_RETURN) {
        return false;
    }
    
    for (size_t i = 0; i < (sizeof(OpReturnPrefixes) / sizeof(OpReturnPrefixes[0])); ++i)
    {
        if (0 == memcmp(&(OpReturnPrefixes[i].prefix)[0],
                &scriptPubKey[2], OpReturnPrefixes[i].prefix.size()))
        {
            typeRet = TX_NULL_DATA;
            metaRet = OpReturnPrefixes[i].name;            
            return true;
        }
    }
    
    return false;
};

static bool IsLoadedScriptSigOfP2PKH(const CScript& scriptSig, txnouttype& typeRet, metatype& metaRet)
{
    opcodetype opcode;
    std::vector<unsigned char> vch;
    std::vector<unsigned char>::const_iterator pc = scriptSig.begin();
    
    // Skip first
    if (!scriptSig.GetOp(pc, opcode, vch)) {
        return false;
    }
    
    // Get second
    if (!scriptSig.GetOp(pc, opcode, vch)) {
        return false;
    }
    
    // Is this a public key?
    if (!((vch.size() == 33 && (vch[0] == 0x02 || vch[0] == 0x03))
            || (vch.size() == 65 && vch[0] == 0x04)))
    {
        return false;
    }

    CKeyID id = CPubKey(vch).GetID();
    uint32_t pfx = ntohl(*(uint32_t*)&id);    

    for (size_t i = 0; i < (sizeof(PubKeyHashPrefixes) / sizeof(PubKeyHashPrefixes[0])); ++i)
    {
        if (pfx >= PubKeyHashPrefixes[i].begin && pfx <= PubKeyHashPrefixes[i].end)
        {
            typeRet = TX_PUBKEYHASH;
            metaRet = PubKeyHashPrefixes[i].name;
            return true;
        }
    }

    return false;
};

static bool IsLoadedScriptPubKey(const CScript& scriptPubKey, txnouttype& typeRet, metatype& metaRet)
{
    return
            IsLoadedOpReturn(scriptPubKey, typeRet, metaRet) ||
            IsLoadedMultisig(scriptPubKey, typeRet, metaRet) ||
            IsLoadedPubKeyHash(scriptPubKey, typeRet, metaRet);
};

static bool CheckTxForPayload(const CTransaction& tx, txnouttype& typeRet, metatype& metaRet)
{
    // Crawl outputs
    BOOST_FOREACH(const CTxOut& txout, tx.vout)
    {
        if (IsLoadedScriptPubKey(txout.scriptPubKey, typeRet, metaRet)) {
            return true;
        }
    }

    // Crawl inputs and check, if it's a spent from a listed entry
    BOOST_FOREACH(const CTxIn& txin, tx.vin)
    {
        if (IsLoadedScriptSigOfP2PKH(txin.scriptSig, typeRet, metaRet)) {
            return true;
        }
    }

    return false;
};

static int64_t GetOutputValue(const uint256& hash, const unsigned int& n)
{
    CTransaction tx;
    uint256 hashBlock = 0;
    
    if (!GetTransaction(hash, tx, hashBlock, true)) {
        return 0;
    }
    
    if (n < tx.vout.size()) {
        return tx.vout[n].nValue;
    }
    
    return 0;
};

static void GetFeeFor(const CTransaction& tx, int64_t& nFeeRet)
{    
    BOOST_FOREACH(const CTxOut& txout, tx.vout)
    {
        nFeeRet -= txout.nValue;
    }
    
    BOOST_FOREACH(const CTxIn& txin, tx.vin)
    {
        nFeeRet += GetOutputValue(txin.prevout.hash, txin.prevout.n);
    }
};

static unsigned int GetTransactionSize(const CTransaction& tx)
{
    return ::GetSerializeSize(tx, SER_NETWORK, PROTOCOL_VERSION);
};

static std::string GetCsvHeadline()
{
    std::ostringstream os;    
    os << "First;Last;Total;Interval;";

    for (int i = 4; i < _DATA_ARRAY_SIZE; ++i) {
        const metatype& type = IntToMetaType(i);
        os << GetMetaType(type) << ";";
    }
    
    return os.str();
};

static std::string ArrayToCsv(uint32_t cData[_DATA_ARRAY_SIZE])
{
    std::ostringstream os;

    for (size_t i = 0; i < _DATA_ARRAY_SIZE; ++i) {
        os << cData[i] << ";";
    }

    return os.str();
};

static std::string ArrayToCsv(uint64_t cData[_DATA_ARRAY_SIZE])
{
    std::ostringstream os;

    for (size_t i = 0; i < _DATA_ARRAY_SIZE; ++i) {
        os << cData[i] << ";";
    }

    return os.str();
};

static void ClearArray(uint32_t arrData[_DATA_ARRAY_SIZE])
{
    for (size_t i = 0; i < _DATA_ARRAY_SIZE; ++i) {
        arrData[i] = 0;
    }
};

static void ClearArray(uint64_t arrData[_DATA_ARRAY_SIZE])
{
    for (size_t i = 0; i < _DATA_ARRAY_SIZE; ++i) {
        arrData[i] = 0;
    }
};

static uint64_t SumArray(uint64_t arrData[_DATA_ARRAY_SIZE])
{
    uint64_t nRet = 0;
    
    for (size_t i = 0; i < _DATA_ARRAY_SIZE; ++i) {
        nRet += arrData[i];
    }
    
    return nRet;
};

Value getmetatransactions(const Array& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw std::runtime_error("getmetatransactions height ( count )");
    
    int nHeight = params[0].get_int();
    if (nHeight < 0 || nHeight > chainActive.Height())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Block height out of range");

    int nCount = 1;
    if (params.size() > 1)
        nCount = params[1].get_int();

    int nLast = nHeight + nCount;
    CBlockIndex* pblockindex = chainActive[nHeight];

    Array response;
    Array responseTxs;  // Holds data about number of transactions
    Array responseFee;  // Holds data about transaction fees
    Array responseLen;  // Holds data about transaction size
    
    std::string csv_header = GetCsvHeadline();
    responseTxs.push_back(csv_header);
    responseFee.push_back(csv_header);
    responseLen.push_back(csv_header);

    uint32_t arrData[_DATA_ARRAY_SIZE] = {};
    uint64_t arrFees[_DATA_ARRAY_SIZE] = {};
    uint64_t arrSize[_DATA_ARRAY_SIZE] = {};
    
    // Used to make sure first or last line isn't duplicated
    bool fLastDump = false;

    arrData[0] = nHeight;  // Beginning of an interval
    arrData[1] = nHeight;  // End of an interval

    while (pblockindex && nHeight < nLast)
    {
        CBlock block;
        if (!ReadBlockFromDisk(block, pblockindex))
            throw JSONRPCError(RPC_INTERNAL_ERROR, "Can't read block from disk");
        
        // Transaction fee of uncategorized transactions are derived from
        // all fees paid in a block.
        
        fLastDump = false;
        int64_t nCoinBaseFee = 0;
        GetFeeFor(block.vtx[0], nCoinBaseFee);
        nCoinBaseFee = -GetBlockValue(nHeight, nCoinBaseFee);

        int nOtherPos = MetaTypeToInt(META_OTHER);
        arrData[nOtherPos]++;
        arrFees[nOtherPos] += nCoinBaseFee;
        arrSize[nOtherPos] += GetTransactionSize(block.vtx[0]);
        
        // Number of transactions per interval
        arrData[3]++;
            
        for (size_t idx = 1; idx < block.vtx.size(); ++idx)
        {
            // Number of transactions per interval
            arrData[3]++;

            CTransaction& tx = block.vtx[idx];
            txnouttype type;
            metatype meta = META_OTHER;
            int64_t nFee = 0;
            
            // Only the fee for categorized transactions is fetched individually
            if (CheckTxForPayload(tx, type, meta)) {
                GetFeeFor(tx, nFee);
            }
            
            int pos = MetaTypeToInt(meta);
            arrData[pos]++;
            arrFees[pos] += nFee;
            arrSize[pos] += GetTransactionSize(tx);
        }        

        nHeight = pblockindex->nHeight;
        arrData[1] = pblockindex->nHeight;
        arrData[2] = pblockindex->nChainTx;

        if ((nHeight + 1) % _DUMP_LINE_EVERY_N_BLOCKS == 0) {
            std::string csv_line = ArrayToCsv(arrData);
            std::string csv_fees = ArrayToCsv(arrFees);
            std::string csv_size = ArrayToCsv(arrSize);
            
            printf("Last processed block: %d, Num: %u, Fee: %.8LF BTC, Size: %.3LF kB\n",
                    nHeight,
                    arrData[3],
                    static_cast<long double>(SumArray(arrFees)) * 0.00000001L,
                    static_cast<long double>(SumArray(arrSize)) / 1000.0L);
            
            responseTxs.push_back(csv_line);
            responseFee.push_back(csv_fees);
            responseLen.push_back(csv_size);
            
            ClearArray(arrFees);
            ClearArray(arrData);
            ClearArray(arrSize);
            
            arrData[0] = nHeight + 1;
            fLastDump = true;
        }

        nHeight++;
        pblockindex = chainActive.Next(pblockindex);
    }
    
    // Finish, if data was dumped already
    if (fLastDump) {
        return response;
    }

    std::string csv_line = ArrayToCsv(arrData);
    std::string csv_fees = ArrayToCsv(arrFees);
    std::string csv_size = ArrayToCsv(arrSize);

    responseTxs.push_back(csv_line);
    responseFee.push_back(csv_fees);
    responseLen.push_back(csv_size);
    
    response.push_back(responseTxs);
    response.push_back(responseFee);
    response.push_back(responseLen);
    
    printf("Last processed block: %d, Num: %u, Fee: %.8LF BTC, Size: %.3LF kB\n",
            nHeight - 1,
            arrData[3],
            static_cast<long double>(SumArray(arrFees)) * 0.00000001L,
            static_cast<long double>(SumArray(arrSize)) / 1000.0L);

    return response;
};

#undef _DATA_ARRAY_SIZE
#undef _DUMP_LINE_EVERY_N_BLOCKS
