#!/usr/bin/env python2
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
from test_framework import BitcoinTestFramework
from util import assert_equal, initialize_datadir
import os
import subprocess


def pretty_dict(dictionary):
    ret = ''
    for key in dictionary:
        if len(ret):
            ret += ', '
        ret += key + '=' + dictionary[key]
    return ret


class LocaleTest(BitcoinTestFramework):
    """Tests whether Bitcoin Core can handle invalid locale environemnt settings."""

    def setup_chain(self):
        pass

    def setup_network(self, split=False):
        self.nodes = []

    def run_test(self):
        self.errors = 0
        self.test_environment({'LC_ALL': 'C', 'LANG': 'en_US.UTF-8'})
        self.test_environment({'LC_ALL': '', 'LANG': ''})
        self.test_environment({'LC_ALL': '', 'LANG': 'invalid'})
        self.test_environment({'LC_ALL': 'invalid', 'LANG': ''})
        assert_equal(self.errors, 0)

    def test_environment(self, env):
        print('Testing with environment variables: %s' % pretty_dict(env))
        test_env = os.environ.copy()
        test_env.update(env)

        self.test_bitcoind(test_env)
        self.test_bitcoin_test(test_env)
        self.test_bitcoin_qt_test(test_env)

    def test_bitcoind(self, env=None):
        file_bitcoind = os.path.abspath(os.path.join(self.options.srcdir, 'bitcoind'))
        file_bitcoin_cli = os.path.abspath(os.path.join(self.options.srcdir, 'bitcoin-cli'))

        if not os.path.isfile(file_bitcoind):
            print('Skipping test of bitcoind, because file not found: ' + file_bitcoind)
            return
        elif not os.path.isfile(file_bitcoin_cli):
            print('Skipping test of bitcoind, because RPC client not found: ' + file_bitcoin_cli)
            return

        datadir = initialize_datadir(self.options.tmpdir, 1)
        print('Initialized data directory ' + datadir)

        print('Starting bitcoind ' + file_bitcoind)
        args = [file_bitcoind, '-datadir=' + datadir, '-keypool=1', '-discover=0']
        proc = subprocess.Popen(args, env=env)

        print('Stopping bitcoind')
        args = [file_bitcoin_cli, '-datadir=' + datadir, '-rpcwait', 'stop']
        subprocess.Popen(args, env=env)

        print('Waiting until bitcoind terminates')
        proc.wait()

        print('bitcoind terminiated with exit status %s' % proc.returncode)
        if proc.returncode != 0:
            self.errors += 1
        # assert_equal(proc.returncode, 0)

    def test_bitcoin_test(self, env=None):
        file_test_bitcoin = os.path.abspath(os.path.join(self.options.srcdir, 'test', 'test_bitcoin'))

        if not os.path.isfile(file_test_bitcoin):
            print('Skipping test of test_bitcoin, because file not found: ' + file_test_bitcoin)
            return

        print('Starting test_bitcoin ' + file_test_bitcoin)
        proc = subprocess.Popen(file_test_bitcoin, env=env)

        print('Waiting until test_bitcoin terminates')
        proc.wait()

        print('test_bitcoin terminiated with exit status %s' % proc.returncode)
        if proc.returncode != 0:
            self.errors += 1
        # assert_equal(proc.returncode, 0)

    def test_bitcoin_qt_test(self, env=None):
        file_test_qt = os.path.abspath(os.path.join(self.options.srcdir, 'qt', 'test', 'test_bitcoin-qt'))

        if not os.path.isfile(file_test_qt):
            print('Skipping test of test_bitcoin-qt, because file not found: ' + file_test_qt)
            return

        print('Starting test_bitcoin-qt ' + file_test_qt)
        proc = subprocess.Popen(file_test_qt, env=env)

        print('Waiting until test_bitcoin-qt terminates')
        proc.wait()

        print('test_bitcoin-qt terminiated exit status %s' % proc.returncode)
        if proc.returncode != 0:
            self.errors += 1
        # assert_equal(proc.returncode, 0)


if __name__ == '__main__':
    LocaleTest().main()
