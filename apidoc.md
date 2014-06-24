Master Core API Documentation
=============================

##Introduction
Master Core is a fork of Bitcoin Core, with Master Protocol feature support added as a new layer of functionality on top.  As such interacting with the API is done in the same manner (JSON-RPC) as Bitcoin Core, simply with additional RPC calls available for utlizing Master Protocol features.

As all existing Bitcoin Core functionality is inherent to Master Core, the RPC port by default remains as 8332 as per Bitcoin Core.  If you wish to run Master Core in tandem with Bitcoin Core (eg via a seperate datadir) you may utilize the -rpcport<port> option to nominate an alternative port number.

*Caution: This document is a work in progress and is subject to change at any time.  There may be errors, omissions or inaccuracies present.*

###Simple Send
Simple send allows a Master Protocol currency to be transferred from address to address in a one-to-one transaction.  Simple send transactions are exposed via the **send_MP** RPC call.

**Required Parameters**
- *sender address (string):* A valid bitcoin address containing a sufficient balance to support the transaction
- *recipient address (string):* A valid bitcoin address - the receiving party of the transaction
- *currency/property ID (integer):* A valid Master Protocol currency/property ID
- *amount (float):* The amount to transfer (note if sending individisble tokens any decimals will be truncated)
   
**Additional Optional Parameters**
There are currently no supported optional parameters for this call.

**Example**




*Please note, the private key for the requested sender address must be available in the wallet.*




