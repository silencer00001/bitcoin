Bitcoin Core address-indexed/extended
=====================================

http://www.bitcoin.org

Copyright (c) 2009-2013 Bitcoin Core Developers

Modifications
-------------

This branch is based on [release v0.9.1](https://github.com/bitcoin/bitcoin/tree/0.9.1) and includes the required changes to allow extended transaction lookups based on an address.

### Setup and configuration

Use `-addrindex=1` to enable address-based indexing of transactions.
Reindexing via `-reindex` is required the first time.

### RPC commands

The following new query commands are available:

```
> listalltransactions "address" (verbose skip count)

Description:
Returns array of all transactions associated with address.

Arguments:
1. address (string, required) The Bitcoin address
2. verbose (numeric, optional, default=0) If 0, return only transaction hex
3. skip (numeric, optional, default=0) The number of transactions to skip
4. count (numeric, optional, default=100) The number of transactions to return
```
```
> listallunspent "address" (verbose minconf maxconf maxreqsigs)

Description:
Returns array of unspent transaction outputs with between minconf and maxconf (inclusive) 
confirmations spendable by the provided address whereby maximal maxreqsigs signatures are 
required to redeem the output.

Arguments:
1. address (string, required) The Bitcoin address
2. verbose (numeric, optional, default=0) If 0, exclude reqSigs, addresses, 
     scriptPubKey (asm, hex), blockhash, blocktime, blockheight
3. minconf (numeric, optional, default=1) The minimum confirmations to filter
4. maxconf (numeric, optional, default=9999999) The maximum confirmations to filter
5. maxreqsigs (numeric, optional, default=1) The number of signatures required to spend the output
```
```
> getallbalance "address" (minconf maxreqsigs)

Description:
Returns the sum of spendable transaction outputs by address with at least minconf 
confirmations whereby maximal maxreqsigs signatures are allowed to be required to 
redeem an output.

Arguments:
1. address (string, required) The Bitcoin address
2. minconf (numeric, optional, default=1) The minimum confirmations to filter
3. maxreqsigs (numeric, optional, default=1) The number of signatures required to spend an output
```
```
Additional changes:

- Include "spendable" in verbosed transaction information output
- Include "blockheight" in verbosed transaction information output
- Include "position" in verbosed transaction information output
- Remove "time" from verbosed transaction information output
```

What is Bitcoin?
----------------

Bitcoin is an experimental new digital currency that enables instant payments to
anyone, anywhere in the world. Bitcoin uses peer-to-peer technology to operate
with no central authority: managing transactions and issuing money are carried
out collectively by the network. Bitcoin Core is the name of open source
software which enables the use of this currency.

For more information, as well as an immediately useable, binary version of
the Bitcoin Core software, see http://www.bitcoin.org/en/download.

License
-------

Bitcoin Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see http://opensource.org/licenses/MIT.

Development process
-------------------

Developers work in their own trees, then submit pull requests when they think
their feature or bug fix is ready.

If it is a simple/trivial/non-controversial change, then one of the Bitcoin
development team members simply pulls it.

If it is a *more complicated or potentially controversial* change, then the patch
submitter will be asked to start a discussion (if they haven't already) on the
[mailing list](http://sourceforge.net/mailarchive/forum.php?forum_name=bitcoin-development).

The patch will be accepted if there is broad consensus that it is a good thing.
Developers should expect to rework and resubmit patches if the code doesn't
match the project's coding conventions (see [doc/coding.md](doc/coding.md)) or are
controversial.

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/bitcoin/bitcoin/tags) are created
regularly to indicate new official, stable release versions of Bitcoin.

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test. Please be patient and help out, and
remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write unit tests for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run (assuming they weren't disabled in configure) with: `make check`

Every pull request is built for both Windows and Linux on a dedicated server,
and unit and sanity tests are automatically run. The binaries produced may be
used for manual QA testing — a link to them will appear in a comment on the
pull request posted by [BitcoinPullTester](https://github.com/BitcoinPullTester). See https://github.com/TheBlueMatt/test-scripts
for the build/test scripts.

### Manual Quality Assurance (QA) Testing

Large changes should have a test plan, and should be tested by somebody other
than the developer who wrote the code.
See https://github.com/bitcoin/QA/ for how to create a test plan.
