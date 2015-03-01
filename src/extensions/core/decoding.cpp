#include "extensions/core/decoding.h"

// <editor-fold desc="Includes">
#include "extensions/bitcoin/chain.h"
#include "extensions/bitcoin/script.h"
#include "extensions/core/obfuscation.h"
#include "extensions/core/policy.h"
#include "extensions/log.h"
#include "extensions/primitives/transaction.h"
#include "extensions/return_value.h"

#include "amount.h"
#include "base58.h"
#include "main.h"
#include "primitives/transaction.h"
#include "pubkey.h"
#include "script/script.h"
#include "script/standard.h"
#include "utilstrencodings.h"

#include <boost/foreach.hpp>

#include <cstring>
#include <map>
#include <string>
#include <vector>
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Exodus addresses">
inline std::string ExodusAddress() {
    if (IsMainNet()) {
        return "1EXoDusjGwvnjZUyKkxZ4UHEf77z6A5S4P";
    } else {
        return "mpexoDuSkGGqvqrkrjiFng38QPkJQVFyqv";
    }
}

inline std::string MoneyManAddress() {
    return "moneyqMan7uh8FqdCA2BV5yZ8qVrc9ikLP";
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Fundraiser and DEx payment">
static int TXExodusFundraiser(const CTransaction& wtx, const std::string& sender, int64_t ExodusHighestValue, int nBlock, unsigned int nTime) {
    LogPrint("decoding", "%s(), line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);

    return 0;
}

static int DEx_payment(const uint256& txid, unsigned int vout, const std::string& seller, const std::string& buyer, uint64_t BTC_paid, int blockNow, uint64_t* nAmended = NULL) {
    LogPrint("decoding", "%s(), line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);

    return 0;
}
// </editor-fold>

int parseTransaction(bool bRPConly, const CTransaction& tx, int nHeight, unsigned int nIdx, CMPTransaction* pMetaTx, unsigned int nTime)
{
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

    pMetaTx->Set(tx.GetHash(), nHeight, nIdx, nTime);

    // quickly go through the outputs & ensure there is a marker (a send to the Exodus address)
    for (unsigned int i = 0; i < tx.vout.size(); i++)
    {
        outAll += tx.vout[i].nValue;

        CTxDestination dest;

        if (ExtractDestination(tx.vout[i].scriptPubKey, dest)) {
            std::string strAddress = CBitcoinAddress(dest).ToString();

            if (ExodusAddress() == strAddress) {
                ExodusValues[marker_count++] = tx.vout[i].nValue;
            } else if (!IsMainNet() && (MoneyManAddress() == strAddress)) {
                TestNetMoneyValues[getmoney_count++] = tx.vout[i].nValue;
            }
        }
    }
    if (!IsMainNet() && getmoney_count) {
    } else if (!marker_count) {
        return error(-1, "no marker found");
    }

    LogPrint("decoding", "____________________________________________________________________________________________________________________________________\n");
    LogPrint("decoding", "%s(block=%d, %s idx= %d); txid: %s\n", __FUNCTION__, nHeight, DateTimeStrFormat("%Y-%m-%d %H:%M:%S", nTime), nIdx, tx.GetHash().GetHex());

    // now save output addresses & scripts for later use
    // also determine if there is a multisig in there, if so = Class B
    for (unsigned int i = 0; i < tx.vout.size(); i++)
    {
        CTxDestination dest;

        if (ExtractDestination(tx.vout[i].scriptPubKey, dest)) {
            txnouttype whichType;
            bool validType = false;
            if (!GetOutputType(tx.vout[i].scriptPubKey, whichType)) validType = false;
            if (IsAllowedOutputType(whichType, nHeight)) validType = true;

            std::string strAddress = CBitcoinAddress(dest).ToString();

            if ((ExodusAddress() != strAddress) && (validType))
            {
                LogPrint("msc_debug_parser_data", "saving address_data #%d: %s:%s\n", i, strAddress, tx.vout[i].scriptPubKey.ToString());

                // saving for Class A processing or reference
                GetScriptPushes(tx.vout[i].scriptPubKey, script_data);
                address_data.push_back(strAddress);
                value_data.push_back(tx.vout[i].nValue);
            }
        } else {
            // a multisig ?
            txnouttype type;
            std::vector<CTxDestination> vDest;
            int nRequired;

            if (ExtractDestinations(tx.vout[i].scriptPubKey, type, vDest, nRequired)) {
                ++fMultisig;
            }
        }
    }

    LogPrint("msc_debug_parser_data", "address_data.size=%lu\n", address_data.size());
    LogPrint("msc_debug_parser_data", "script_data.size=%lu\n", script_data.size());
    LogPrint("msc_debug_parser_data", "value_data.size=%lu\n", value_data.size());

    // several types of erroroneous MP TX inputs
    int inputs_errors = 0;
    std::map<std::string, uint64_t> inputs_sum_of_values;
    // now go through inputs & identify the sender, collect input amounts
    // go through inputs, find the largest per Mastercoin protocol, the Sender
    for (unsigned int i = 0; i < tx.vin.size(); i++)
    {
        LogPrint("msc_debug_vin", "vin=%d:%s\n", i, tx.vin[i].scriptSig.ToString());

        CTransaction txPrev;
        uint256 hashBlock;
        // get the vin's previous transaction 
        if (!GetTransaction(tx.vin[i].prevout.hash, txPrev, hashBlock, true)) {
            return error(-101, "could not get vin's previous transaction");
        }

        unsigned int n = tx.vin[i].prevout.n;

        CTxDestination source;

        uint64_t nValue = txPrev.vout[n].nValue;
        txnouttype whichType;

        inAll += nValue;

        // extract the destination of the previous transaction's vout[n]
        if (ExtractDestination(txPrev.vout[n].scriptPubKey, source)) 
        {
            // we only allow pay-to-pubkeyhash, pay-to-scripthash & probably pay-to-pubkey (?)
            {
                if (!GetOutputType(txPrev.vout[n].scriptPubKey, whichType)) ++inputs_errors;
                if (!IsAllowedOutputType(whichType, nHeight)) ++inputs_errors;

                if (inputs_errors) break;
            }

            CBitcoinAddress addressSource(source); // convert this to an address
            inputs_sum_of_values[addressSource.ToString()] += nValue;
        } else ++inputs_errors;

        LogPrint("msc_debug_vin", "vin=%d:%s\n", i, tx.vin[i].ToString());
    } // end of inputs for loop

    txFee = inAll - outAll; // this is the fee paid to miners for this TX

    if (inputs_errors) {
        return error(-101, "vin output type not allowed");
    }

    // largest by sum of values
    uint64_t nMax = 0;
    for (std::map<std::string, uint64_t>::iterator it = inputs_sum_of_values.begin(); it != inputs_sum_of_values.end(); ++it) {
        uint64_t nTemp = it->second;

        if (nTemp > nMax) {
            strSender = it->first;
            LogPrint("msc_debug_exo", "looking for The Sender: %s , nMax=%lu, nTemp=%lu\n", strSender, nMax, nTemp);
            nMax = nTemp;
        }
    }

    if (!strSender.empty()) {
        LogPrint("msc_debug_verbose", "The Sender: %s : His Input Sum of Values= %lu.%08lu ; fee= %lu.%08lu\n",
                strSender, nMax / COIN, nMax % COIN, txFee / COIN, txFee % COIN);
    } else {
        LogPrint("decoding", "The sender is still EMPTY !!! txid: %s\n", tx.GetHash().GetHex());
        return error(-5, "no sender identified");
    }

    //This calculates exodus fundraiser for each tx within a given block
    int64_t BTC_amount = ExodusValues[0];
    if (!IsMainNet()) {
        if (MONEYMAN_TESTNET_BLOCK <= nHeight) BTC_amount = TestNetMoneyValues[0];
    }

    if (IsRegTest()) {
        if (MONEYMAN_REGTEST_BLOCK <= nHeight) BTC_amount = TestNetMoneyValues[0];
    }

    if (0 < BTC_amount && !bRPConly) (void) TXExodusFundraiser(tx, strSender, BTC_amount, nHeight, nTime);

    // go through the outputs
    for (unsigned int i = 0; i < tx.vout.size(); i++) {
        CTxDestination address;

        // if TRUE -- non-multisig
        if (ExtractDestination(tx.vout[i].scriptPubKey, address)) {
        } else {
            // probably a multisig -- get them

            txnouttype type;
            std::vector<CTxDestination> vDest;
            int nRequired;

            // CScript is a std::vector
            LogPrint("msc_debug_script", "scriptPubKey: %s\n", HexStr(tx.vout[i].scriptPubKey));

            if (ExtractDestinations(tx.vout[i].scriptPubKey, type, vDest, nRequired)) {
                LogPrint("msc_debug_script", " >> multisig: ");

                BOOST_FOREACH(const CTxDestination& dest, vDest)
                {
                    CBitcoinAddress address = CBitcoinAddress(dest);
                    CKeyID keyID;

                    if (!address.GetKeyID(keyID)) {
                        // TODO: add an error handler
                    }

                    LogPrint("msc_debug_script", "%s ; ", address.ToString());

                }
                LogPrint("msc_debug_script", "\n");

                // ignore first public key, as it should belong to the sender
                // and it be used to avoid the creation of unspendable dust
                GetScriptPushes(tx.vout[i].scriptPubKey, multisig_script_data, true);
            }
        }
    } // end of the outputs' for loop

    std::string strObfuscatedHashes[1 + MAX_SHA256_OBFUSCATION_TIMES];
    PrepareObfuscatedHashes(strSender, strObfuscatedHashes);

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
            if (!GetOutputType(tx.vout[k].scriptPubKey, whichType)) break; // unable to determine type, ignore output
            if (!IsAllowedOutputType(whichType, nHeight)) break;
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
                    LogPrint("msc_debug_parser_data", "Data Address located - data[%d]:%s: %s (%lu.%08lu)\n", k, script_data[k], address_data[k], value_data[k] / COIN, value_data[k] % COIN);
                } else {
                    // invalidate - Class A cannot be more than one data packet - possible collision, treat as default (BTC payment)
                    strDataAddress = ""; //empty strScriptData to block further parsing
                    LogPrint("msc_debug_parser_data", "Multiple Data Addresses found (collision?) Class A invalidated, defaulting to BTC payment\n");
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
                if (!GetOutputType(tx.vout[k].scriptPubKey, whichType)) break; // unable to determine type, ignore output
                if (!IsAllowedOutputType(whichType, nHeight)) break;

                seq = (ParseHex(script_data[k].substr(0, 2)))[0]; // retrieve sequence number

                if ((address_data[k] != strDataAddress) && (address_data[k] != ExodusAddress()) && (expectedRefAddressSeq == seq)) // found reference address with matching sequence number
                {
                    if (strRefAddress.empty()) // confirm we have not already located a reference address
                    {
                        strRefAddress = address_data[k]; // set ref address
                        LogPrint("msc_debug_parser_data", "Reference Address located via seqnum - data[%d]:%s: %s (%lu.%08lu)\n", k, script_data[k], address_data[k], value_data[k] / COIN, value_data[k] % COIN);
                    } else {
                        // can't trust sequence numbers to provide reference address, there is a collision with >1 address with expected seqnum
                        strRefAddress = ""; // blank ref address
                        LogPrint("msc_debug_parser_data", "Reference Address sequence number collision, will fall back to evaluating matching output amounts\n");
                        break;
                    }
                }
            }
            // Step 3, if we still don't have a reference address, see if we can locate an address with matching output amounts
            if (strRefAddress.empty()) {
                for (unsigned k = 0; k < script_data.size(); k++) // loop through outputs
                {
                    txnouttype whichType;
                    if (!GetOutputType(tx.vout[k].scriptPubKey, whichType)) break; // unable to determine type, ignore output
                    if (!IsAllowedOutputType(whichType, nHeight)) break;

                    if ((address_data[k] != strDataAddress) && (address_data[k] != ExodusAddress()) && (dataAddressValue == value_data[k])) // this output matches data output, check if matches exodus output
                    {
                        for (int exodus_idx = 0; exodus_idx < marker_count; exodus_idx++) {
                            if (value_data[k] == ExodusValues[exodus_idx]) //this output matches data address value and exodus address value, choose as ref
                            {
                                if (strRefAddress.empty()) {
                                    strRefAddress = address_data[k];
                                    LogPrint("msc_debug_parser_data", "Reference Address located via matching amounts - data[%d]:%s: %s (%lu.%08lu)\n", k, script_data[k], address_data[k], value_data[k] / COIN, value_data[k] % COIN);
                                } else {
                                    strRefAddress = "";
                                    LogPrint("msc_debug_parser_data", "Reference Address collision, multiple potential candidates. Class A invalidated, defaulting to BTC payment\n");
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
            LogPrint("decoding", "!! sender: %s , receiver: %s\n", strSender, strReference);
            LogPrint("decoding", "!! this may be the BTC payment for an offer !!\n");

            // TODO collect all payments made to non-itself & non-exodus and their amounts -- these may be purchases!!!

            int count = 0;
            // go through the outputs, once again...
            {
                for (unsigned int i = 0; i < tx.vout.size(); i++) {
                    CTxDestination dest;

                    if (ExtractDestination(tx.vout[i].scriptPubKey, dest)) {
                        const std::string strAddress = CBitcoinAddress(dest).ToString();

                        if (ExodusAddress() == strAddress) continue;
                        LogPrint("decoding", "payment #%d %s %11.8lf\n", count, strAddress, (double) tx.vout[i].nValue / (double) COIN);

                        // check everything & pay BTC for the property we are buying here...
                        if (bRPConly) count = 55555; // no real way to validate a payment during simple RPC call
                        else if (0 == DEx_payment(tx.GetHash(), i, strAddress, strSender, tx.vout[i].nValue, nHeight)) ++count;
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
            LogPrint("msc_debug_parser_data", "valid Class A:from=%s:to=%s:data=%s\n", strSender, strReference, strScriptData);
            packet_size = PACKET_SIZE_CLASS_A;
            memcpy(single_pkt, &ParseHex(strScriptData)[0], packet_size);
        }
    } else // if (fMultisig)
    {
        unsigned int k = 0;
        // gotta find the Reference - Z rewrite - scrappy & inefficient, can be optimized

        LogPrint("msc_debug_parser_data", "Beginning reference identification\n");

        bool referenceFound = false; // bool to hold whether we've found the reference yet
        bool changeRemoved = false; // bool to hold whether we've ignored the first output to sender as change
        unsigned int potentialReferenceOutputs = 0; // int to hold number of potential reference outputs

        // how many potential reference outputs do we have, if just one select it right here

        BOOST_FOREACH(const std::string &addr, address_data)
        {
            // keep Michael's original debug info & k int as used elsewhere
            LogPrint("msc_debug_parser_data", "ref? data[%d]:%s: %s (%lu.%08lu)\n",
                    k, script_data[k], addr, value_data[k] / COIN, value_data[k] % COIN);
            ++k;

            if (addr != ExodusAddress()) {
                ++potentialReferenceOutputs;
                if (1 == potentialReferenceOutputs) {
                    strReference = addr;
                    referenceFound = true;
                    LogPrint("msc_debug_parser_data", "Single reference potentially id'd as follows: %s \n", strReference);
                } else //as soon as potentialReferenceOutputs > 1 we need to go fishing
                {
                    strReference = ""; // avoid leaving strReference populated for sanity
                    referenceFound = false;
                    LogPrint("msc_debug_parser_data", "More than one potential reference candidate, blanking strReference, need to go fishing\n");
                }
            }
        }

        // do we have a reference now? or do we need to dig deeper
        if (!referenceFound) // multiple possible reference addresses
        {
            LogPrint("msc_debug_parser_data", "Reference has not been found yet, going fishing\n");

            BOOST_FOREACH(const std::string &addr, address_data)
            {
                // !!!! address_data is ordered by vout (i think - please confirm that's correct analysis?)
                if (addr != ExodusAddress()) // removed strSender restriction, not to spec
                {
                    if ((addr == strSender) && (!changeRemoved)) {
                        // per spec ignore first output to sender as change if multiple possible ref addresses
                        changeRemoved = true;
                        LogPrint("msc_debug_parser_data", "Removed change\n");
                    } else {
                        // this may be set several times, but last time will be highest vout
                        strReference = addr;
                        LogPrint("msc_debug_parser_data", "Resetting strReference as follows: %s \n ", strReference);
                    }
                }
            }
        }

        LogPrint("msc_debug_parser_data", "Ending reference identification\n");
        LogPrint("msc_debug_parser_data", "Final decision on reference identification is: %s\n", strReference);

        // multisig , Class B; get the data packets that are found here
        for (unsigned int k = 0; k < multisig_script_data.size(); k++) {
            CPubKey key(ParseHex(multisig_script_data[k]));
            CKeyID keyID = key.GetID();
            std::string strAddress = CBitcoinAddress(keyID).ToString();
            char *c_addr_type = (char *) "";
            std::string strPacket;

            {
                // this is a data packet, must deobfuscate now
                std::vector<unsigned char> hash = ParseHex(strObfuscatedHashes[mdata_count + 1]);
                std::vector<unsigned char> packet = ParseHex(multisig_script_data[k].substr(2 * 1, 2 * PACKET_SIZE));

                for (unsigned int i = 0; i < packet.size(); i++) {
                    packet[i] ^= hash[i];
                }

                memcpy(&packets[mdata_count], &packet[0], PACKET_SIZE);
                strPacket = HexStr(packet.begin(), packet.end(), false);
                ++mdata_count;

                if (MAX_PACKETS <= mdata_count) {
                    LogPrint("decoding", "increase MAX_PACKETS ! mdata_count= %d\n", mdata_count);
                    return error(-222, "too many packets found");
                }
            }

            LogPrint("msc_debug_parser_data", "multisig_data[%d]:%s: %s%s\n", k, multisig_script_data[k], strAddress, c_addr_type);

            if (!strPacket.empty()) LogPrint("msc_debug_parser", "packet #%d: %s\n", mdata_count, strPacket);
        }

        packet_size = mdata_count * (PACKET_SIZE - 1);

        if (sizeof (single_pkt) < packet_size) {
            return error(-111, "packet size does not add up");
        }

    } // end of if (fMultisig)

    // now decode mastercoin packets
    for (int m = 0; m < mdata_count; m++) {
        LogPrint("msc_debug_parser", "m=%d: %s\n", m, HexStr(packets[m], PACKET_SIZE + packets[m]));

        // check to ensure the sequence numbers are sequential and begin with 01 !
        if (1 + m != packets[m][0]) {
            LogPrint("msc_debug_spec", "Error: non-sequential seqnum ! expected=%d, got=%d\n", 1 + m, packets[m][0]);
        }

        // now ignoring sequence numbers for Class B packets
        memcpy(m * (PACKET_SIZE - 1) + single_pkt, 1 + packets[m], PACKET_SIZE - 1);
    }

    LogPrint("msc_debug_verbose", "single_pkt: %s\n", HexStr(single_pkt, packet_size + single_pkt));

    pMetaTx->Set(strSender, strReference, 0, tx.GetHash(), nHeight, nIdx, (unsigned char *) &single_pkt, packet_size, fMultisig, (inAll - outAll));

    return success(0, "transaction decomposed");
}
