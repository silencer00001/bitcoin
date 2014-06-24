Master Core API Documentation
=============================

##Introduction
Master Core is a fork of Bitcoin Core, with Master Protocol feature support added as a new layer of functionality on top.  As such interacting with the API is done in the same manner (JSON-RPC) as Bitcoin Core, simply with additional RPC calls available for utlizing Master Protocol features.

As all existing Bitcoin Core functionality is inherent to Master Core, the RPC port by default remains as 8332 as per Bitcoin Core.  If you wish to run Master Core in tandem with Bitcoin Core (eg via a seperate datadir) you may utilize the -rpcport<port> option to nominate an alternative port number.

*Caution: This document is a work in progress and is subject to change at any time.  There may be errors, omissions or inaccuracies present.*

###Simple Send
Simple send allows a Master Protocol currency to be transferred from address to address in a one-to-one transaction.  Simple send transactions are exposed via the **send_MP** RPC call.

**Required Parameters**
 * **sender address (string):** A valid bitcoin address containing a sufficient balance to support the transaction
 * **recipient address (string):** A valid bitcoin address - the receiving party of the transaction
 * **currency/property ID (integer):** A valid Master Protocol currency/property ID
 * **amount (float):** The amount to transfer (note if sending individisble tokens any decimals will be truncated)
   
**Additional Optional Parameters**
- There are currently no supported optional parameters for this call.

**Examples**
```
$src/mastercored send_MP 1MCHESTxYkPSLoJ57WBQot7vz3xkNahkcb 1MCHESTbJhJK27Ygqj4qKkx4Z4ZxhnP826 1 0.005
7a06ef1bb62230b04f4b7839f3b677397629cd433c03a0534d7ba57500efec1f

{"jsonrpc":"1.0","id":"1","method":"send_MP","params":["1MCHESTxYkPSLoJ57WBQot7vz3xkNahkcb","1MCHESTbJhJK27Ygqj4qKkx4Z4ZxhnP826",1,0.005]}
{"result":"7a06ef1bb62230b04f4b7839f3b677397629cd433c03a0534d7ba57500efec1f","error":null,"id":"1"}
```
*Please note, the private key for the requested sender address must be available in the wallet.*

###getbalance_MP
The getbalance_MP call allows for retrieval of a Master Protocol balance for a given address and currency/property ID.

**Required Parameters**
- **address (string):** A valid bitcoin address 
- **currency/property ID (integer):** A valid Master Protocol currency/property ID

**Additional Optional Parameters**
- There are currently no supported optional parameters for this call.

**Examples**
```
$src/mastercored getbalance_MP mvKKTXj8Z1GVwjN1Ejw8yx6n7pBujdXG2Q 1
300000000.00000000

{"jsonrpc":"1.0","id":"1","method":"getbalance_MP","params":["mvKKTXj8Z1GVwjN1Ejw8yx6n7pBujdXG2Q",1]}
{"result":300000000.00000000,"error":null,"id":"1"}
```

###gettransaction_MP

**Examples**
```
$src/mastercored gettransaction_MP d2907fe2c716fc6d510d63b52557907445c784cb2e8ae6ea9ef61e909c978cd7
{
    "txid" : "d2907fe2c716fc6d510d63b52557907445c784cb2e8ae6ea9ef61e909c978cd7",
    "sendingaddress" : "myN6HXmFhmMRo1bzfNXBDxTALYsh3EjXxk",
    "referenceaddress" : "mhgrKJ3WyX1RMYiUpgA3M3iF48zSeSRkri",
    "direction" : "out",
    "confirmations" : 884,
    "fee" : 0.00010000,
    "blocktime" : 1403298479,
    "blockindex" : 49,
    "type" : "Simple Send",
    "currency" : 1,
    "divisible" : true,
    "amount" : 50.00000000,
    "valid" : true
}

{"jsonrpc":"1.0","id":"1","method":"gettransaction_MP","params":["d2907fe2c716fc6d510d63b52557907445c784cb2e8ae6ea9ef61e909c978cd7"]}
{"result":{"txid":"d2907fe2c716fc6d510d63b52557907445c784cb2e8ae6ea9ef61e909c978cd7","sendingaddress":"myN6HXmFhmMRo1bzfNXBDxTALYsh3EjXxk","referenceaddress":"mhgrKJ3WyX1RMYiUpgA3M3iF48zSeSRkri","direction":"out","confirmations":884,"fee":0.00010000,"blocktime":1403298479,"blockindex":49,"type":"Simple Send","currency":1,"divisible":true,"amount":50.00000000,"valid":true},"error":null,"id":"1"}
```
*Please note, gettransaction_MP currently supports transactions available in the wallet only.*

###listtransactions_MP

**Examples**
```
$src/mastercored listtransactions_MP mtGfANEnFsniGzWDt87kQg4zJunoQbT6f3
[
    {
        "txid" : "fda128e34edc48426ca930df6167e4560cef9cda2192e37be69c965e9c5dd9d1",
        "address" : "mtGfANEnFsniGzWDt87kQg4zJunoQbT6f3",
        "direction" : "in",
        "confirmations" : 1453,
        "blocktime" : 1403126898,
        "blockindex" : 7,
        "type" : "Simple Send",
        "currency" : 1,
        "divisible" : true,
        "amount" : 123456.00000000,
        "valid" : true
    },
    {
        "txid" : "33e4ea9a43102f9ad43b086d2bcf9478c67b5a1e64ce7dfc64bfe3f94b7f9222",
        "address" : "mtGfANEnFsniGzWDt87kQg4zJunoQbT6f3",
        "direction" : "in",
        "confirmations" : 1450,
        "blocktime" : 1403129492,
        "blockindex" : 4,
        "type" : "Simple Send",
        "currency" : 1,
        "divisible" : true,
        "amount" : 222.00000000,
        "valid" : true
    },
    {
        "txid" : "c93a8622b6784b4cd5e109bea423553ed729b675965b6820837f80513be04852",
        "address" : "mtGfANEnFsniGzWDt87kQg4zJunoQbT6f3",
        "direction" : "out",
        "confirmations" : 902,
        "blocktime" : 1403293908,
        "blockindex" : 2,
        "type" : "Simple Send",
        "currency" : 1,
        "divisible" : true,
        "amount" : 50.12340000,
        "valid" : true
    }
]

{"jsonrpc":"1.0","id":"1","method":"listtransactions_MP","params":["mtGfANEnFsniGzWDt87kQg4zJunoQbT6f3"]}
{"result":[{"txid":"fda128e34edc48426ca930df6167e4560cef9cda2192e37be69c965e9c5dd9d1","address":"mtGfANEnFsniGzWDt87kQg4zJunoQbT6f3","direction":"in","confirmations":1453,"blocktime":1403126898,"blockindex":7,"type":"Simple Send","currency":1,"divisible":true,"amount":123456.00000000,"valid":true},{"txid":"33e4ea9a43102f9ad43b086d2bcf9478c67b5a1e64ce7dfc64bfe3f94b7f9222","address":"mtGfANEnFsniGzWDt87kQg4zJunoQbT6f3","direction":"in","confirmations":1450,"blocktime":1403129492,"blockindex":4,"type":"Simple Send","currency":1,"divisible":true,"amount":222.00000000,"valid":true},{"txid":"c93a8622b6784b4cd5e109bea423553ed729b675965b6820837f80513be04852","address":"mtGfANEnFsniGzWDt87kQg4zJunoQbT6f3","direction":"out","confirmations":902,"blocktime":1403293908,"blockindex":2,"type":"Simple Send","currency":1,"divisible":true,"amount":50.12340000,"valid":true}],"error":null,"id":"1"}
```
*Please note, listtransactions_MP currently supports transactions available in the wallet only.*



