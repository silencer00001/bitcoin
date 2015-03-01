/*
#include "extensions/encoding.h"

#include "extensions/script.h"
#include "extensions/transactions.h"

#include "base58.h"
#include "chainparams.h"
#include "main.h"
#include "primitives/transaction.h"
#include "pubkey.h"
#include "script/script.h"
#include "script/standard.h"
#include "utilstrencodings.h"
#include "utiltime.h"

#include <openssl/sha.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/foreach.hpp>

#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace extensions {

#define MAX_SHA256_OBFUSCATION_TIMES  255
#define PACKET_SIZE_CLASS_A            19
#define MAX_BTC_OUTPUTS                16

enum BLOCKHEIGHTRESTRICTIONS {
    MONEYMAN_REGTEST_BLOCK= 101,
    MONEYMAN_TESTNET_BLOCK= 270775,
};

// Debug log
static bool msc_debug_parser_data = true;
static bool msc_debug_vin = true;
static bool msc_debug_verbose = true;
static bool msc_debug_script = true;
static bool msc_debug_verbose2 = true;
static bool msc_debug_parser = true;
static bool msc_debug_spec = true;
static bool msc_debug_exo = true;

// Exodus
static std::string exodus_address = "1EXoDusjGwvnjZUyKkxZ4UHEf77z6A5S4P";
static const std::string exodus_testnet = "mpexoDuSkGGqvqrkrjiFng38QPkJQVFyqv";
static const std::string getmoney_testnet = "moneyqMan7uh8FqdCA2BV5yZ8qVrc9ikLP";

static bool RegTest() {
    return "regtest" == Params().NetworkIDString();
}

static bool TestNet() {
    return "testnet" == Params().NetworkIDString();
}

static bool isNonMainNet() {
    return "mainnet" != Params().NetworkIDString();
}

static std::string ExodusAddress() {
    if (isNonMainNet()) {
        exodus_address = exodus_testnet;
    }
}

static bool getOutputType(const CScript& scriptPubKey, txnouttype& whichTypeRet) {
    printf("%s(), line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);

    std::vector<std::vector<unsigned char> > vSolutions;

    if (!Solver(scriptPubKey, whichTypeRet, vSolutions)) return false;

    return true;
}

static bool isAllowedOutputType(int whichType, int nBlock) {
    printf("%s(), line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);

    return (TX_PUBKEYHASH == whichType) || (TX_SCRIPTHASH == whichType);
}

static int TXExodusFundraiser(const CTransaction& wtx, const std::string& sender, int64_t ExodusHighestValue, int nBlock, unsigned int nTime) {
    printf("%s(), line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);

    return 0;
}

static int DEx_payment(const uint256& txid, unsigned int vout, const std::string& seller, const std::string& buyer, uint64_t BTC_paid, int blockNow, uint64_t* nAmended = NULL) {
    printf("%s(), line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);

    return 0;
}

static void prepareObfuscatedHashes(const std::string& address, std::string(&ObfsHashes)[1 + MAX_SHA256_OBFUSCATION_TIMES]) {
    unsigned char sha_input[128];
    unsigned char sha_result[128];
    std::vector<unsigned char> vec_chars;

    strcpy((char *) sha_input, address.c_str());
    // do only as many re-hashes as there are mastercoin packets, 255 per spec
    for (unsigned int j = 1; j <= MAX_SHA256_OBFUSCATION_TIMES; j++) {
        SHA256(sha_input, strlen((const char *) sha_input), sha_result);

        vec_chars.resize(32);
        memcpy(&vec_chars[0], &sha_result[0], 32);
        ObfsHashes[j] = HexStr(vec_chars);
        boost::to_upper(ObfsHashes[j]);

        if (msc_debug_verbose2) if (5 > j) printf("%d: sha256 hex: %s\n", j, ObfsHashes[j].c_str());
        strcpy((char *) sha_input, ObfsHashes[j].c_str());
    }
}

int mastercore_init() {
    printf("%s()%s, line %d, file: %s\n", __FUNCTION__, isNonMainNet() ? "TESTNET" : "", __LINE__, __FILE__);

    printf("\n%s OMNICORE INIT, build date: " __DATE__ " " __TIME__ "\n\n", DateTimeStrFormat("%Y-%m-%d %H:%M:%S", GetTime()).c_str());

    if (isNonMainNet()) {
        exodus_address = exodus_testnet;
    }

    return 0;
}

static int error(int code, const char* message)
{
    printf("Error: %s (code: %d)\n", message, code);
    return code;
}

static int success(int code, const char* message)
{
    printf("Success: %s (code: %d)\n", message, code);
    return code;
}

int parseTransaction(bool bRPConly, const CTransaction& wtx, int nBlock, unsigned int idx, CMPTransaction* mp_tx, unsigned int nTime)
{
    mastercore_init();


    std::string strSender;
    // class A: data & address storage -- combine them into a structure or something
    std::vector<std::string> script_data;
    std::vector<std::string> address_data;
    std::vector<int64_t>value_data;
    int64_t ExodusValues[MAX_BTC_OUTPUTS] = {0};
    int64_t TestNetMoneyValues[MAX_BTC_OUTPUTS] = {0}; // new way to get funded on TestNet, send TBTC to moneyman address
    std::string strReference;
    unsigned char single_pkt[MAX_PACKETS * PACKET_SIZE];
    unsigned int packet_size = 0;
    int fMultisig = 0;
    int marker_count = 0, getmoney_count = 0;
    // class B: multisig data storage
    std::vector<std::string> multisig_script_data;
    uint64_t inAll = 0;
    uint64_t outAll = 0;
    uint64_t txFee = 0;

    mp_tx->Set(wtx.GetHash(), nBlock, idx, nTime);

    // quickly go through the outputs & ensure there is a marker (a send to the Exodus address)
    for (unsigned int i = 0; i < wtx.vout.size(); i++) {
        CTxDestination dest;
        std::string strAddress;

        outAll += wtx.vout[i].nValue;

        if (ExtractDestination(wtx.vout[i].scriptPubKey, dest)) {
            strAddress = CBitcoinAddress(dest).ToString();

            if (exodus_address == strAddress) {
                ExodusValues[marker_count++] = wtx.vout[i].nValue;
            } else if (isNonMainNet() && (getmoney_testnet == strAddress)) {
                TestNetMoneyValues[getmoney_count++] = wtx.vout[i].nValue;
            }
        }
    }
    if ((isNonMainNet() && getmoney_count)) {
    } else if (!marker_count) {
        return error(-1, "no marker found");
    }

    printf("____________________________________________________________________________________________________________________________________\n");
    printf("%s(block=%d, %s idx= %d); txid: %s\n", __FUNCTION__, nBlock, DateTimeStrFormat("%Y-%m-%d %H:%M:%S", nTime).c_str(),
            idx, wtx.GetHash().GetHex().c_str());

    // now save output addresses & scripts for later use
    // also determine if there is a multisig in there, if so = Class B
    for (unsigned int i = 0; i < wtx.vout.size(); i++) {
        CTxDestination dest;
        std::string strAddress;

        if (ExtractDestination(wtx.vout[i].scriptPubKey, dest)) {
            txnouttype whichType;
            bool validType = false;
            if (!getOutputType(wtx.vout[i].scriptPubKey, whichType)) validType = false;
            if (isAllowedOutputType(whichType, nBlock)) validType = true;

            strAddress = CBitcoinAddress(dest).ToString();

            if ((exodus_address != strAddress) && (validType)) {
                if (msc_debug_parser_data) printf("saving address_data #%d: %s:%s\n", i, strAddress.c_str(), wtx.vout[i].scriptPubKey.ToString().c_str());

                // saving for Class A processing or reference
                GetScriptPushes(wtx.vout[i].scriptPubKey, script_data);
                address_data.push_back(strAddress);
                value_data.push_back(wtx.vout[i].nValue);
            }
        } else {
            // a multisig ?
            txnouttype type;
            std::vector<CTxDestination> vDest;
            int nRequired;

            if (ExtractDestinations(wtx.vout[i].scriptPubKey, type, vDest, nRequired)) {
                ++fMultisig;
            }
        }
    }

    if (msc_debug_parser_data) {
        printf("address_data.size=%lu\n", address_data.size());
        printf("script_data.size=%lu\n", script_data.size());
        printf("value_data.size=%lu\n", value_data.size());
    }

    int inputs_errors = 0; // several types of erroroneous MP TX inputs
    std::map<std::string, uint64_t> inputs_sum_of_values;
    // now go through inputs & identify the sender, collect input amounts
    // go through inputs, find the largest per Mastercoin protocol, the Sender
    for (unsigned int i = 0; i < wtx.vin.size(); i++) {
        CTxDestination address;

        if (msc_debug_vin) printf("vin=%d:%s\n", i, wtx.vin[i].scriptSig.ToString().c_str());

        CTransaction txPrev;
        uint256 hashBlock;
        if (!GetTransaction(wtx.vin[i].prevout.hash, txPrev, hashBlock, true)) // get the vin's previous transaction 
        {
            return error(-101, "could not get vin's previous transaction");
        }

        unsigned int n = wtx.vin[i].prevout.n;

        CTxDestination source;

        uint64_t nValue = txPrev.vout[n].nValue;
        txnouttype whichType;

        inAll += nValue;

        if (ExtractDestination(txPrev.vout[n].scriptPubKey, source)) // extract the destination of the previous transaction's vout[n]
        {
            // we only allow pay-to-pubkeyhash, pay-to-scripthash & probably pay-to-pubkey (?)
            {
                if (!getOutputType(txPrev.vout[n].scriptPubKey, whichType)) ++inputs_errors;
                if (!isAllowedOutputType(whichType, nBlock)) ++inputs_errors;

                if (inputs_errors) break;
            }

            CBitcoinAddress addressSource(source); // convert this to an address

            inputs_sum_of_values[addressSource.ToString()] += nValue;
        } else ++inputs_errors;

        if (msc_debug_vin) printf("vin=%d:%s\n", i, wtx.vin[i].ToString().c_str());
    } // end of inputs for loop

    txFee = inAll - outAll; // this is the fee paid to miners for this TX

    if (inputs_errors) // not a valid MP TX
    {
        return error(-101, "vin output type not allowed");
    }

    // largest by sum of values
    uint64_t nMax = 0;
    for (std::map<std::string, uint64_t>::iterator my_it = inputs_sum_of_values.begin(); my_it != inputs_sum_of_values.end(); ++my_it) {
        uint64_t nTemp = my_it->second;

        if (nTemp > nMax) {
            strSender = my_it->first;
            if (msc_debug_exo) printf("looking for The Sender: %s , nMax=%lu, nTemp=%lu\n", strSender.c_str(), nMax, nTemp);
            nMax = nTemp;
        }
    }

    if (!strSender.empty()) {
        if (msc_debug_verbose) printf("The Sender: %s : His Input Sum of Values= %lu.%08lu ; fee= %lu.%08lu\n",
                strSender.c_str(), nMax / COIN, nMax % COIN, txFee / COIN, txFee % COIN);
    } else {
        printf("The sender is still EMPTY !!! txid: %s\n", wtx.GetHash().GetHex().c_str());
        return error(-5, "no sender identified");
    }

    //This calculates exodus fundraiser for each tx within a given block
    int64_t BTC_amount = ExodusValues[0];
    if (isNonMainNet()) {
        if (MONEYMAN_TESTNET_BLOCK <= nBlock) BTC_amount = TestNetMoneyValues[0];
    }

    if (RegTest()) {
        if (MONEYMAN_REGTEST_BLOCK <= nBlock) BTC_amount = TestNetMoneyValues[0];
    }

    //            printf("%s() amount = %ld , nBlock = %d, line %d, file: %s\n", __FUNCTION__, BTC_amount, nBlock, __LINE__, __FILE__);

    if (0 < BTC_amount && !bRPConly) (void) TXExodusFundraiser(wtx, strSender, BTC_amount, nBlock, nTime);

    // go through the outputs
    for (unsigned int i = 0; i < wtx.vout.size(); i++) {
        CTxDestination address;

        // if TRUE -- non-multisig
        if (ExtractDestination(wtx.vout[i].scriptPubKey, address)) {
        } else {
            // probably a multisig -- get them

            txnouttype type;
            std::vector<CTxDestination> vDest;
            int nRequired;

            // CScript is a std::vector
            if (msc_debug_script) printf("scriptPubKey: %s\n", HexStr(wtx.vout[i].scriptPubKey).c_str());

            if (ExtractDestinations(wtx.vout[i].scriptPubKey, type, vDest, nRequired)) {
                if (msc_debug_script) printf(" >> multisig: ");

                BOOST_FOREACH(const CTxDestination &dest, vDest)
                {
                    CBitcoinAddress address = CBitcoinAddress(dest);
                    CKeyID keyID;

                    if (!address.GetKeyID(keyID)) {
                        // TODO: add an error handler
                    }

                    // base_uint is a superclass of dest, size(), GetHex() is the same as ToString()
                    //              printf("%s size=%d (%s); ", address.ToString().c_str(), keyID.size(), keyID.GetHex().c_str());
                    if (msc_debug_script) printf("%s ; ", address.ToString().c_str());

                }
                if (msc_debug_script) printf("\n");

                // ignore first public key, as it should belong to the sender
                // and it be used to avoid the creation of unspendable dust
                GetScriptPushes(wtx.vout[i].scriptPubKey, multisig_script_data, true);
            }
        }
    } // end of the outputs' for loop

    std::string strObfuscatedHashes[1 + MAX_SHA256_OBFUSCATION_TIMES];
    prepareObfuscatedHashes(strSender, strObfuscatedHashes);

    unsigned char packets[MAX_PACKETS][32];
    int mdata_count = 0; // multisig data count
    if (!fMultisig) {
        // ---------------------------------- Class A parsing ---------------------------

        // Init vars
        std::string strScriptData;
        std::string strDataAddress;
        std::string strRefAddress;
        unsigned char dataAddressSeq = 0xFF;
        unsigned char seq = 0xFF;
        int64_t dataAddressValue = 0;

        // Step 1, locate the data packet
        for (unsigned k = 0; k < script_data.size(); k++) // loop through outputs
        {
            txnouttype whichType;
            if (!getOutputType(wtx.vout[k].scriptPubKey, whichType)) break; // unable to determine type, ignore output
            if (!isAllowedOutputType(whichType, nBlock)) break;
            std::string strSub = script_data[k].substr(2, 16); // retrieve bytes 1-9 of packet for peek & decode comparison
            seq = (ParseHex(script_data[k].substr(0, 2)))[0]; // retrieve sequence number

            if (("0000000000000001" == strSub) || ("0000000000000002" == strSub)) // peek & decode comparison
            {
                if (strScriptData.empty()) // confirm we have not already located a data address
                {
                    strScriptData = script_data[k].substr(2 * 1, 2 * PACKET_SIZE_CLASS_A); // populate data packet
                    strDataAddress = address_data[k]; // record data address
                    dataAddressSeq = seq; // record data address seq num for reference matching
                    dataAddressValue = value_data[k]; // record data address amount for reference matching
                    if (msc_debug_parser_data) printf("Data Address located - data[%d]:%s: %s (%lu.%08lu)\n", k, script_data[k].c_str(), address_data[k].c_str(), value_data[k] / COIN, value_data[k] % COIN);
                } else {
                    // invalidate - Class A cannot be more than one data packet - possible collision, treat as default (BTC payment)
                    strDataAddress = ""; //empty strScriptData to block further parsing
                    if (msc_debug_parser_data) printf("Multiple Data Addresses found (collision?) Class A invalidated, defaulting to BTC payment\n");
                    break;
                }
            }
        }

        // Step 2, see if we can locate an address with a seqnum +1 of DataAddressSeq
        if (!strDataAddress.empty()) // verify Step 1, we should now have a valid data packet, if so continue parsing
        {
            unsigned char expectedRefAddressSeq = dataAddressSeq + 1;
            for (unsigned k = 0; k < script_data.size(); k++) // loop through outputs
            {
                txnouttype whichType;
                if (!getOutputType(wtx.vout[k].scriptPubKey, whichType)) break; // unable to determine type, ignore output
                if (!isAllowedOutputType(whichType, nBlock)) break;

                seq = (ParseHex(script_data[k].substr(0, 2)))[0]; // retrieve sequence number

                if ((address_data[k] != strDataAddress) && (address_data[k] != exodus_address) && (expectedRefAddressSeq == seq)) // found reference address with matching sequence number
                {
                    if (strRefAddress.empty()) // confirm we have not already located a reference address
                    {
                        strRefAddress = address_data[k]; // set ref address
                        if (msc_debug_parser_data) printf("Reference Address located via seqnum - data[%d]:%s: %s (%lu.%08lu)\n", k, script_data[k].c_str(), address_data[k].c_str(), value_data[k] / COIN, value_data[k] % COIN);
                    } else {
                        // can't trust sequence numbers to provide reference address, there is a collision with >1 address with expected seqnum
                        strRefAddress = ""; // blank ref address
                        if (msc_debug_parser_data) printf("Reference Address sequence number collision, will fall back to evaluating matching output amounts\n");
                        break;
                    }
                }
            }
            // Step 3, if we still don't have a reference address, see if we can locate an address with matching output amounts
            if (strRefAddress.empty()) {
                for (unsigned k = 0; k < script_data.size(); k++) // loop through outputs
                {
                    txnouttype whichType;
                    if (!getOutputType(wtx.vout[k].scriptPubKey, whichType)) break; // unable to determine type, ignore output
                    if (!isAllowedOutputType(whichType, nBlock)) break;

                    if ((address_data[k] != strDataAddress) && (address_data[k] != exodus_address) && (dataAddressValue == value_data[k])) // this output matches data output, check if matches exodus output
                    {
                        for (int exodus_idx = 0; exodus_idx < marker_count; exodus_idx++) {
                            if (value_data[k] == ExodusValues[exodus_idx]) //this output matches data address value and exodus address value, choose as ref
                            {
                                if (strRefAddress.empty()) {
                                    strRefAddress = address_data[k];
                                    if (msc_debug_parser_data) printf("Reference Address located via matching amounts - data[%d]:%s: %s (%lu.%08lu)\n", k, script_data[k].c_str(), address_data[k].c_str(), value_data[k] / COIN, value_data[k] % COIN);
                                } else {
                                    strRefAddress = "";
                                    if (msc_debug_parser_data) printf("Reference Address collision, multiple potential candidates. Class A invalidated, defaulting to BTC payment\n");
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        } // end if (!strDataAddress.empty())

        // Populate expected var strReference with chosen address (if not empty)
        if (!strRefAddress.empty()) strReference = strRefAddress;

        // Last validation step, if strRefAddress is empty, blank strDataAddress so we default to BTC payment
        if (strRefAddress.empty()) strDataAddress = "";

        // -------------------------------- End Class A parsing -------------------------

        if (strDataAddress.empty()) // an empty Data Address here means it is not Class A valid and should be defaulted to a BTC payment
        {
            // this must be the BTC payment - validate (?)
            //              if (msc_debug_verbose) printf("\n================BLOCK: %d======\ntxid: %s\n", nBlock, wtx.GetHash().GetHex().c_str());
            printf("!! sender: %s , receiver: %s\n", strSender.c_str(), strReference.c_str());
            printf("!! this may be the BTC payment for an offer !!\n");

            // TODO collect all payments made to non-itself & non-exodus and their amounts -- these may be purchases!!!

            int count = 0;
            // go through the outputs, once again...
            {
                for (unsigned int i = 0; i < wtx.vout.size(); i++) {
                    CTxDestination dest;

                    if (ExtractDestination(wtx.vout[i].scriptPubKey, dest)) {
                        const std::string strAddress = CBitcoinAddress(dest).ToString();

                        if (exodus_address == strAddress) continue;
                        printf("payment #%d %s %11.8lf\n", count, strAddress.c_str(), (double) wtx.vout[i].nValue / (double) COIN);

                        // check everything & pay BTC for the property we are buying here...
                        if (bRPConly) count = 55555; // no real way to validate a payment during simple RPC call
                        else if (0 == DEx_payment(wtx.GetHash(), i, strAddress, strSender, wtx.vout[i].nValue, nBlock)) ++count;
                    }
                }
            }
            if (count) {
                // return count -- the actual number of payments within this TX or error if none were made
                return success(count, "identified bitcoin payment");
            } else {
                return error(-5678, "no valid meta transaction and not a bitcoin payment");
            }
        } else {
            // valid Class A packet almost ready
            if (msc_debug_parser_data) printf("valid Class A:from=%s:to=%s:data=%s\n", strSender.c_str(), strReference.c_str(), strScriptData.c_str());
            packet_size = PACKET_SIZE_CLASS_A;
            memcpy(single_pkt, &ParseHex(strScriptData)[0], packet_size);
        }
    } else // if (fMultisig)
    {
        unsigned int k = 0;
        // gotta find the Reference - Z rewrite - scrappy & inefficient, can be optimized

        if (msc_debug_parser_data) printf("Beginning reference identification\n");

        bool referenceFound = false; // bool to hold whether we've found the reference yet
        bool changeRemoved = false; // bool to hold whether we've ignored the first output to sender as change
        unsigned int potentialReferenceOutputs = 0; // int to hold number of potential reference outputs

        // how many potential reference outputs do we have, if just one select it right here

        BOOST_FOREACH(const std::string &addr, address_data)
        {
            // keep Michael's original debug info & k int as used elsewhere
            if (msc_debug_parser_data) printf("ref? data[%d]:%s: %s (%lu.%08lu)\n",
                    k, script_data[k].c_str(), addr.c_str(), value_data[k] / COIN, value_data[k] % COIN);
            ++k;

            if (addr != exodus_address) {
                ++potentialReferenceOutputs;
                if (1 == potentialReferenceOutputs) {
                    strReference = addr;
                    referenceFound = true;
                    if (msc_debug_parser_data) printf("Single reference potentially id'd as follows: %s \n", strReference.c_str());
                } else //as soon as potentialReferenceOutputs > 1 we need to go fishing
                {
                    strReference = ""; // avoid leaving strReference populated for sanity
                    referenceFound = false;
                    if (msc_debug_parser_data) printf("More than one potential reference candidate, blanking strReference, need to go fishing\n");
                }
            }
        }

        // do we have a reference now? or do we need to dig deeper
        if (!referenceFound) // multiple possible reference addresses
        {
            if (msc_debug_parser_data) printf("Reference has not been found yet, going fishing\n");

            BOOST_FOREACH(const std::string &addr, address_data)
            {
                // !!!! address_data is ordered by vout (i think - please confirm that's correct analysis?)
                if (addr != exodus_address) // removed strSender restriction, not to spec
                {
                    if ((addr == strSender) && (!changeRemoved)) {
                        // per spec ignore first output to sender as change if multiple possible ref addresses
                        changeRemoved = true;
                        if (msc_debug_parser_data) printf("Removed change\n");
                    } else {
                        // this may be set several times, but last time will be highest vout
                        strReference = addr;
                        if (msc_debug_parser_data) printf("Resetting strReference as follows: %s \n ", strReference.c_str());
                    }
                }
            }
        }

        if (msc_debug_parser_data) printf("Ending reference identification\n");
        if (msc_debug_parser_data) printf("Final decision on reference identification is: %s\n", strReference.c_str());

        if (msc_debug_parser) printf("%s(), line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);
        // multisig , Class B; get the data packets that are found here
        for (unsigned int k = 0; k < multisig_script_data.size(); k++) {
            if (msc_debug_parser) printf("%s(), line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);
            CPubKey key(ParseHex(multisig_script_data[k]));
            CKeyID keyID = key.GetID();
            std::string strAddress = CBitcoinAddress(keyID).ToString();
            char *c_addr_type = (char *) "";
            std::string strPacket;

            if (msc_debug_parser) printf("%s(), line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);
            {
                // this is a data packet, must deobfuscate now
                std::vector<unsigned char>hash = ParseHex(strObfuscatedHashes[mdata_count + 1]);
                std::vector<unsigned char>packet = ParseHex(multisig_script_data[k].substr(2 * 1, 2 * PACKET_SIZE));

                for (unsigned int i = 0; i < packet.size(); i++) {
                    packet[i] ^= hash[i];
                }

                memcpy(&packets[mdata_count], &packet[0], PACKET_SIZE);
                strPacket = HexStr(packet.begin(), packet.end(), false);
                ++mdata_count;

                if (MAX_PACKETS <= mdata_count) {
                    printf("increase MAX_PACKETS ! mdata_count= %d\n", mdata_count);
                    return error(-222, "too many packets found");
                }
            }

            if (msc_debug_parser_data) printf("multisig_data[%d]:%s: %s%s\n", k, multisig_script_data[k].c_str(), strAddress.c_str(), c_addr_type);

            if (!strPacket.empty()) {
                if (msc_debug_parser) printf("packet #%d: %s\n", mdata_count, strPacket.c_str());
            }
            if (msc_debug_parser) printf("%s(), line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);
        }

        packet_size = mdata_count * (PACKET_SIZE - 1);

        if (sizeof (single_pkt) < packet_size) {
            return error(-111, "packet size does not add up");
        }

        if (msc_debug_parser) printf("%s(), line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);
    } // end of if (fMultisig)
    if (msc_debug_parser) printf("%s(), line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);

    // now decode mastercoin packets
    for (int m = 0; m < mdata_count; m++) {
        if (msc_debug_parser) printf("m=%d: %s\n", m, HexStr(packets[m], PACKET_SIZE + packets[m], false).c_str());

        // check to ensure the sequence numbers are sequential and begin with 01 !
        if (1 + m != packets[m][0]) {
            if (msc_debug_spec) printf("Error: non-sequential seqnum ! expected=%d, got=%d\n", 1 + m, packets[m][0]);
        }

        // now ignoring sequence numbers for Class B packets
        memcpy(m * (PACKET_SIZE - 1) + single_pkt, 1 + packets[m], PACKET_SIZE - 1);
    }

    if (msc_debug_verbose) printf("single_pkt: %s\n", HexStr(single_pkt, packet_size + single_pkt, false).c_str());

    mp_tx->Set(strSender, strReference, 0, wtx.GetHash(), nBlock, idx, (unsigned char *) &single_pkt, packet_size, fMultisig, (inAll - outAll));

    return success(0, "transaction decomposed");
}

} // namespace extensions
*/