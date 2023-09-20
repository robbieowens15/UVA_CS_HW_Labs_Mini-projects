#!/usr/bin/python3

# This is the homework submission file for the BTC Scripting homework, which
# can be found at http://aaronbloomfield.github.io/ccc/hws/btcscript.  That
# page describes how to fill in this program.


from bitcoin.wallet import CBitcoinAddress, CBitcoinSecret
from bitcoin import SelectParams
from bitcoin.core import CMutableTransaction
from bitcoin.core.script import *
from bitcoin.core import x


#------------------------------------------------------------
# Do not touch: change nothing in this section!

# ensure we are using the bitcoin testnet and not the real bitcoin network
SelectParams('testnet')

# The address that we will pay our tBTC to -- do not change this!
tbtc_return_address = CBitcoinAddress('mwL32bFWqjymzgcQmQ9rdH34VPi8fAfsWh')

# The address that we will pay our BCY to -- do not change this!
bcy_dest_address = CBitcoinAddress('mgBT4ViPjTTcbnLn9SFKBRfGtBGsmaqsZz')

# Yes, we want to broadcast transactions
broadcast_transactions = True

# Ensure we don't call this directly
if __name__ == '__main__':
    print("This script is not meant to be called directly -- call bitcoinctl.py instead")
    exit()


#------------------------------------------------------------
# Setup: your information

# Your UVA userid
userid = 'rao7utn'

# Enter the BTC private key and invoice address from the setup 'Testnet Setup'
# section of the assignment.  
my_private_key_str = "cVBpGaZwyJtUx2brfwQF2meoAXDycK5tC4h8DXhpEdABUuTPFuWi"
my_invoice_address_str = "n1rVMBnfJ8Gvym8SVbA46CZtSn4zTDaNGk"

#? Is this right?
from bitcoin.wallet import CBitcoinSecret, P2PKHBitcoinAddress
from bitcoin import SelectParams
SelectParams('testnet')
private_key = CBitcoinSecret(my_private_key_str)
public_key = private_key.pub
address = P2PKHBitcoinAddress.from_pubkey(public_key)

# Enter the transaction ids (TXID) from the funding part of the 'Testnet
# Setup' section of the assignment.  Each of these was provided from a faucet
# call.  And obviously replace the empty string in the list with the first
# one you botain..
txid_funding_list = [
        "f44825dfb5e44c2a03cdd153214b6da270aaa8234c505cc31ec71baee0e4069c",
        "b2d0bd01f655722b712bfa3a1de983efd294aa886c0b8eaad454a5bff40be31b"
    ]

#! Don't change this -- it is so we can easily access the first such funding
# transaction.
txid_initial = txid_funding_list[0]

# These conversions are so that you can use them more easily in the functions
# below -- don't change these two lines.
if my_private_key_str != "":
    my_private_key = CBitcoinSecret(my_private_key_str)
    my_public_key = my_private_key.pub


#------------------------------------------------------------
# Utility function(s)

# This function will create a signature of a given transaction.  The
# transaction itself is passed in via the first three parameters, and the key
# to sign it with is the last parameter.  The parameters are:
# - txin: the transaction input of the transaction being signed; type: CMutableTxIn
# - txout: the transaction output of the transaction being signed; type: CMutableTxOut
# - txin_scriptPubKey: the pubKey script of the transaction being signed; type: list
# - private_key: the private key to sign the transaction; type: CBitcoinSecret
def create_CHECKSIG_signature(txin, txout, txin_scriptPubKey, private_key):
    tx = CMutableTransaction([txin], [txout])
    sighash = SignatureHash(CScript(txin_scriptPubKey), tx, 0, SIGHASH_ALL)
    return private_key.sign(sighash) + bytes([SIGHASH_ALL])


#------------------------------------------------------------
# Testnet Setup: splitting coins

# The transaction ID that is to be split -- the assumption is that it is the
# transaction hash, above, that funded your account with tBTC.  If you are
# splitting a different faucet transaction, then change this appropriately.
# It must have been paid to the address that corresponds to the private key
# above
split_txid = txid_funding_list[0]

# How much BTC is in that UTXO; look this up on https://live.blockcypher.com
# to get the correct amount.
split_amount_to_split = 0.0005

# How many UTXO indices to split it into.  Note that it will actually split
# into one less, and use the last one as the transaction fee.
split_into_n = int(split_amount_to_split/0.0001)

# The transaction IDs obtained after successfully splitting the tBTC.
txid_split_list = [
    "d20307d02084a17f1cd55274413fff9f5aa6fdc5c50d94ac536038078864b77d",
    "5cda8e7b623fbb6948f40f3c5547935fa30670cd197afcfd0d710618965aa080"
    ]

#! Don't change this -- it's so some of our legacy grading code still works.
txid_split = txid_split_list[1] #!must be 0 when submitting!


#------------------------------------------------------------
# Global settings: some of these will need to be changed for EACH RUN

# The transaction ID that is being redeemed for the various parts herein --
# this should be the result of the split transaction, above; thus, the
# default is probably sufficient.
txid_utxo = txid_split

# The index of the UTXO that is being spent -- note that these indices are
# indexed from 0.  Note that you will have to change this for EACH run, as
# once a UTXO index is spent, it can't be spent again.  If there is only one
# index, then this should be set to 0.
utxo_index = 0

# How much tBTC to send -- this should be LESS THAN the amount in that
# particular UTXO index -- if it's not less than the amount in the UTXO, then
# there is no miner fee, and it will not be mined into a block.  Setting it
# to 90% of the value of the UTXO index is reasonable.  Note that the amount
# in a UTXO index is split_amount_to_split / split_into_n.
send_amount = 0.00009


#------------------------------------------------------------
# Part 1: P2PKH transaction

# This defines the pubkey script (aka output script) for the transaction you
# are creating.  This should be a standard P2PKH script.  The parameter is:
# - address: the address this transaction is being paid to; type:
#   P2PKHBitcoinAddress
def P2PKH_scriptPubKey(address):
    return [ 
             OP_DUP, 
             OP_HASH160,
             address,
             OP_EQUALVERIFY,
             OP_CHECKSIG
           ]

# This function provides the sigscript (aka input script) for the transaction
# that is being redeemed.  This is for a standard P2PKH script.  The
# parameters are:
# - txin: the transaction input of the UTXO being redeemed; type:
#   CMutableTxIn
# - txout: the transaction output of the UTXO being redeemed; type:
#   CMutableTxOut
# - txin_scriptPubKey: the pubKey script (aka output script) of the UTXO being
#   redeemed; type: list
# - private_key: the private key of the redeemer of the UTXO; type:
#   CBitcoinSecret
def P2PKH_scriptSig(txin, txout, txin_scriptPubKey, private_key):
    check_sig = create_CHECKSIG_signature(txin, txout, txin_scriptPubKey, private_key)
    return [ 
            check_sig,
            private_key.pub
           ]

# The transaction hash received after the successful execution of this part
txid_p2pkh = "b28426ce654526054650ae19d83d6157f8b9ed3ce46bfc069dbb5d8c1644f3c7"


#------------------------------------------------------------
# Part 2: puzzle transaction

# These two values are constants that you should choose -- they should be four
# digits long.  They need to allow for only integer solutions to the linear
# equations specified in the assignment.
puzzle_txn_p = 2000
puzzle_txn_q = 2000

# These are the solutions to the linear equations specified in the homework
# assignment.  You can use an online linear equation solver to find the
# solutions.
puzzle_txn_x = 500
puzzle_txn_y = 500

# This function provides the pubKey script (aka output script) that requres a
# solution to the above equations to redeem this UTXO.
def puzzle_scriptPubKey():
    return [ 
            OP_2DUP,
            OP_SWAP,
            OP_DUP,
            OP_DUP,
            OP_ADD,
            OP_ADD,
            OP_ADD,
            puzzle_txn_p,
            OP_EQUALVERIFY,
            OP_DUP,
            OP_DUP,
            OP_ADD,
            OP_ADD,
            OP_ADD,
            puzzle_txn_q,
            OP_EQUAL
           ]

# This function provides the sigscript (aka input script) for the transaction
# that you are redeeming.  It should only provide the two values x and y, but
# in the order of your choice.
def puzzle_scriptSig():
    return [ 
             puzzle_txn_x,
             puzzle_txn_y
           ]

# The transaction hash received after successfully submitting the first
# transaction above (part 2a)
txid_puzzle_txn1 = "7dd434254e9f8ea2092f5d2331ba2a19a2283d90e9a9edd039934832eb0376aa"

# The transaction hash received after successfully submitting the second
# transaction above (part 2b)
txid_puzzle_txn2 = "23f4f3972a877cdcd07c7ebca16c9311215ae15b529aa7f5fcc421f0ac765d40"


#------------------------------------------------------------
# Part 3: Multi-signature transaction

# These are the public and private keys that need to be created for alice,
# bob, and charlie
alice_private_key_str = "cPPuVDEAf49p2AutdA6q26BTsZJwPMkAJDktzEpaEms9JmBZPFVc"
alice_invoice_address_str = "muvaeLbLC87nc9v2wbYG1DcV6F49zVz4Ap"
bob_private_key_str = "cSkwyrhmCrYKdov99kp4eM3ifwvVmyHkdVWvmiA8YGgzj7wRW7ED"
bob_invoice_address_str = "mq4i3G1XUHzT98d5dwAjxSaK5xPaUzZ1ns"
charlie_private_key_str = "cPpxJmyP2wqob7kXeM56TN6Ej5zGKMN59DGxpppxhqmF2jLKjFLj"
charlie_invoice_address_str = "mn4t48EitknPLj6c8YcQT5hvadREZtGZa7"

# These three lines convert the above strings into the type that is usable in
# a script -- you should NOT modify these lines.
if alice_private_key_str != "":
    alice_private_key = CBitcoinSecret(alice_private_key_str)
if bob_private_key_str != "":
    bob_private_key = CBitcoinSecret(bob_private_key_str)
if charlie_private_key_str != "":
    charlie_private_key = CBitcoinSecret(charlie_private_key_str)

# This function provides the pubKey script (aka output script) that will
# require multiple different keys to allow redeeming this UTXO.  It MUST use
# the OP_CHECKMULTISIGVERIFY opcode.  While there are no parameters to the
# function, you should use the keys above for alice, bob, and charlie, as
# well as your own key.
def multisig_scriptPubKey():
    return [ 
            alice_private_key.pub,
            bob_private_key.pub,
            charlie_private_key.pub,
            OP_3,
            OP_CHECKMULTISIGVERIFY,
            OP_CHECKSIG
           ]

# This function provides the sigScript (aka input script) that can redeem the
# above transaction.  The parameters are the same as for P2PKH_scriptSig
# (), above.  You also will need to use the keys for alice, bob, and charlie,
# as well as your own key.  The private key parameter used is the global
# my_private_key.
def multisig_scriptSig(txin, txout, txin_scriptPubKey):
    bank_sig = create_CHECKSIG_signature(txin, txout, txin_scriptPubKey, my_private_key)
    alice_sig = create_CHECKSIG_signature(txin, txout, txin_scriptPubKey, alice_private_key)
    bob_sig = create_CHECKSIG_signature(txin, txout, txin_scriptPubKey, bob_private_key)
    charlie_sig = create_CHECKSIG_signature(txin, txout, txin_scriptPubKey, charlie_private_key)
    return [
            bank_sig,
            my_private_key.pub,
            OP_0,
            alice_sig, 
            bob_sig,
            OP_2
           ]

# The transaction hash received after successfully submitting the first
# transaction above (part 3a)
txid_multisig_txn1 = "4a6161a00a40785d9760e9b0c29d97d76b7894cd94d3af8568ad63d26d93e9cc"

# The transaction hash received after successfully submitting the second
# transaction above (part 3b)
txid_multisig_txn2 = "f6ecd8fd445f08d28a76a340970a180d5397e3c589e05624a35e4b1155178f36"


#------------------------------------------------------------
# Part 4: cross-chain transaction

# This is the API token obtained after creating an account on
# https://accounts.blockcypher.com/.  This is optional!  But you may want to
# keep it here so that everything is all in once place.
blockcypher_api_token = "1dd61840aeae4d3882fd9cfeb9ef1fd8"

# These are the private keys and invoice addresses obtained on the BCY test
# network.
my_private_key_bcy_str = "92aa33ed135a5ac4bd49210fa7e8e58bc72ce4a3c0f32aede11a40618b97fbce"
my_invoice_address_bcy_str = "Bw2E8cr2n3xpioeCBirnXmNAd3WCMiUQ7C"
bob_private_key_bcy_str = "8da9e83314f12ba52c11f8eaf0b7a64840cddfefd7f8ba41a3fb6490ccfa67ad"
bob_invoice_address_bcy_str = "C3HhjtMDScLdzKg99cMde4PvAijQ6FN2U3"

# This is the transaction hash for the funding transaction for Bob's BCY
# network wallet.
txid_bob_bcy_funding = "a8adf891d5f02d0aafc5c4b06f5348c5677f6264beb67625e6e41793a34addd2"

# This is the transaction hash for the split transaction for the trasnaction
# above.
txid_bob_bcy_split = "ad36c14f6fc7c01bba1a67fd94ec8a91e44d07574c2c1f6e0534ce8eeb39abed"

# This is the secret used in this atomic swap.  It needs to be between 1 million
# and 2 billion.
atomic_swap_secret = 2 * 10 ** 7

# This function provides the pubKey script (aka output script) that will set
# up the atomic swap.  This function is run by both Alice (aka you) and Bob,
# but on different networks (tBTC for you/Alice, and BCY for Bob).  This is
# used to create TXNs 1 and 3, which are described at
# http://aaronbloomfield.github.io/ccc/slides/bitcoin.html#/xchainpt1.
def atomicswap_scriptPubKey(public_key_sender, public_key_recipient, hash_of_secret):
    return [ 
            OP_IF,
            OP_HASH160,
            hash_of_secret,
            OP_EQUALVERIFY,
            public_key_recipient,
            OP_CHECKSIG,
            OP_ELSE,
            public_key_sender,
            OP_CHECKMULTISIGVERIFY,
            OP_ENDIF
           ]

# This is the ScriptSig that the receiver will use to redeem coins.  It's
# provided in full so that you can write the atomicswap_scriptPubKey()
# function, above.  This creates the "normal" redeeming script, shown in steps 5 and 6 at 
# http://aaronbloomfield.github.io/ccc/slides/bitcoin.html#/atomicsteps.
def atomcswap_scriptSig_redeem(sig_recipient, secret):
    return [
        sig_recipient, secret, OP_TRUE,
    ]

# This is the ScriptSig for sending coins back to the sender if unredeemed; it
# is provided in full so that you can write the atomicswap_scriptPubKey()
# function, above.  This is used to create TXNs 2 and 4, which are
# described at
# http://aaronbloomfield.github.io/ccc/slides/bitcoin.html#/xchainpt1.  In
# practice, this would be time-locked in the future -- it would include a
# timestamp and call OP_CHECKLOCKTIMEVERIFY.  Because the time can not be
# known when the assignment is written, and as it will vary for each student,
# that part is omitted.
def atomcswap_scriptSig_refund(sig_sender, sig_recipient):
    return [
        sig_recipient, sig_sender, OP_FALSE,
    ]

# The transaction hash received after successfully submitting part 4a
txid_atomicswap_alice_send_tbtc = "18a0dd90330ca4da967ed2b99d8595ea52ac1bd1090ef735290beeb048fdd6a8"

# The transaction hash received after successfully submitting part 4b
txid_atomicswap_bob_send_bcy = "b02db034dfeae7734c6c9c747d46bae4775120255abb81b35b195df18fb6ec5a"

# The transaction hash received after successfully submitting part 4c
txid_atomicswap_alice_redeem_bcy = "91cd73cfada107311c9b2d466aec02c547ce05785e2457eb100832607542ff5d"

# The transaction hash received after successfully submitting part 4d
txid_atomicswap_bob_redeem_tbtc = "7ceed92a854779ccb80081378583ac4b852a520204a5784e2621065f75a5bd25"


#------------------------------------------------------------
# part 5: return everything to the faucet

# nothing to fill in here, as we are going to look at the balance of
# `my_invoice_address_str` to verify that you've completed this part.