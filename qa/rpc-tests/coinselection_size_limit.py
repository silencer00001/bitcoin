#!/usr/bin/env python2
# Copyright (c) 2015 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from test_framework import BitcoinTestFramework
from util import *
import logging
logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.INFO)


class CoinSelectionLimitTest(BitcoinTestFramework):
    """
    Tests whether the wallet coin selection and transaction creation is able to select a subset of available coins,
    such that the size of the created transaction is less than MAX_STANDARD_TX_SIZE (100000).

    Scenario 1 (not coded):
        - The actor starts with 1x 50.0 BTC and 1000x 0.05 BTC (100.0 BTC total) unspent outputs
        - Then tries to spend 49.5 BTC
        - It is expected that the 50.0 BTC output is selected, because the transaction would otherwise be too large

    Scenario 2 (not coded):
        - The actor starts with 500x 0.1 BTC and 1000x 0.05 BTC (100.0 BTC total) unspent outputs
        - Then tries to spend 49.5 BTC
        - It is expected that a combination of coins is selected, such that the created transaction is not too large

    Scenario 3 (coded below):
        - The actor starts with 1x 50.0 BTC, 500x 0.1 BTC and 1000x 0.05 BTC (150.0 BTC total) unspent outputs
        - Then tries to spend 49.5 BTC
        - It is expected that a combination of coins is selected, such that the created transaction is not too large

    For each scenario at least one solution exists in theory, where a combination of coins results in a transaction
    that has a size of less than MAX_STANDARD_TX_SIZE.

    The selected coins are dumped into the debug.log of node 3 (default: /tmp/testXXXX/node3/regtest/debug.log).
    """

    def setup_chain(self):
        logging.info('Initializing test directory ' + self.options.tmpdir)
        initialize_chain_clean(self.options.tmpdir, 4)

    def setup_network(self, split=False):
        extra_args = [['-debug=selectcoins'], ['-debug=selectcoins'], ['-debug=selectcoins'], ['-debug=selectcoins']]
        self.nodes = start_nodes(4, self.options.tmpdir, extra_args)
        connect_nodes(self.nodes[0], 3)
        connect_nodes(self.nodes[1], 3)
        connect_nodes(self.nodes[2], 3)
        connect_nodes(self.nodes[2], 0)
        self.is_network_split = False
        self.sync_all()

    def create_many_outputs(self, from_node, to_node, number_of_outputs, output_value_each):
        inputs = []
        outputs = {}
        utxo = from_node.listunspent()

        total_in = Decimal('0.00000000')
        while len(utxo) > 0:
            t = utxo.pop()
            total_in += t['amount']
            inputs.append({'txid': t['txid'], 'vout': t['vout']})

        total_out = Decimal('0.00000000')
        for n in range(0, number_of_outputs):
            total_out += output_value_each
            # Create a new address for each output to ensure the transaction
            # really has the desired number of outputs
            destination = to_node.getnewaddress()
            outputs[destination] = output_value_each

        if total_in < total_out:
            raise RuntimeError('Insufficient funds: need %s, have %s' % (total_out, total_in,))

        tx_unsigned = from_node.createrawtransaction(inputs, outputs)
        sign_result = from_node.signrawtransaction(tx_unsigned)

        # Don't care about any change
        txid = from_node.sendrawtransaction(sign_result['hex'], True)
        return txid

    def run_test(self):
        logging.info('Generate 105 blocks from node 0 (the miner)')
        self.nodes[0].setgenerate(True, 105)
        self.sync_all()

        logging.info('Send 100.0 BTC from node 0 to node 1')
        self.nodes[0].sendtoaddress(self.nodes[1].getnewaddress(), Decimal('100.0'))

        logging.info('Send 100.0 BTC from node 0 to node 2')
        self.nodes[0].sendtoaddress(self.nodes[2].getnewaddress(), Decimal('100.0'))

        logging.info('Generate 1 block from node 0 (the miner)')
        self.sync_all()
        self.nodes[0].setgenerate(True, 1)
        self.sync_all()

        logging.info('Send raw transaction from node 0 to node 3 (main actor) with 1x 50.0 BTC output')
        self.create_many_outputs(self.nodes[0], self.nodes[3], 1, Decimal('50.0'))

        logging.info('Send raw transaction from node 1 to node 3 (main actor) with 500x 0.1 BTC outputs')
        self.create_many_outputs(self.nodes[1], self.nodes[3], 500, Decimal('0.1'))

        logging.info('Send raw transaction from node 2 to node 3 (main actor) with 1000x 0.05 BTC outputs')
        self.create_many_outputs(self.nodes[2], self.nodes[3], 1000, Decimal('0.05'))

        logging.info('Generate 1 block from node 0 (the miner)')
        self.sync_all()
        self.nodes[0].setgenerate(True, 1)
        self.sync_all()

        logging.info('Node 3 (main actor) has a balance of %s BTC' % (self.nodes[3].getbalance(),))
        logging.info('Node 3 (main actor) has a total of %d unspent outputs' % (len(self.nodes[3].listunspent()),))

        logging.info('Try to send 49.5 BTC from node 3 (main actor) to a new address')
        self.nodes[3].sendtoaddress(self.nodes[0].getnewaddress(), Decimal('49.5'))

        # The test passed, if the last command succeeds, and no exception is thrown during the
        # transaction creation, because the created transaction was too large.


if __name__ == '__main__':
    CoinSelectionLimitTest().main()
