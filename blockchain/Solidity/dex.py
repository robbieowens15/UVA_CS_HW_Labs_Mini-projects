# Submission information for the Decentralized Exchange (DEX) HW
# https://aaronbloomfield.github.io/ccc/hws/dex/

# The filename of this file must be 'dex.py', else the submission
# verification routines will not work properly.

# You are welcome to have additional variables or fields in this file; you
# just cant remove variables or fields.


# Who are you?  Name and UVA userid.  The name can be in any human-readable format.
userid = "rao7utn"
name = "Robert Owens"


# eth.coinbase: this is the account that you deployed the smart contracts
# (and performed any necessary transactions) for this assignment.  Be sure to
# include the leading '0x' in the address.
eth_coinbase = "0xf27b9184d2ceb5a297929e42a7bea41f1a00bbad"


# This dictionary contains the contract addresses of the various contracts
# that need to be deployed for this assignment.  The addresses do not need to
# be in checksummed form.  The contracts do, however, need to be deployed by
# the eth_coinbase address, above.  Be sure to include the leading '0x' in
# the address.
contracts = {

	# The Token Cryptocurrency contract.  As you had to make a few changes,
	# you will have had to re-deploy it for this assignment.  The address
	# does not need to be in checksummed form.  It must have been deployed by
	# the eth_coinbase address, above.
	'token_cc': '0x47411218ce06a9eD9AB7e5Ae3b5F25027f188e3D',

	# Your DEX contract.  All of the actions in this file are assumed to be
	# from this contract.  The address does not need to be in checksummed
	# form.  It must have been deployed by the eth_coinbase address, above.
	'dex': '0xD896Da961FB4aA795dE616a6a4CEDfdBAb4a41fc',
}


# This dictionary contains various information that will vary depending on the
# assignment.
other = {
	
	# nothing is required in this dictionary for this assignment

}


# These are various sanity checks, and are meant to help you ensure that you
# submitted everything that you are supposed to submit.  Other than
# submitting the necessary files to Gradescope (which checks for those
# files), all other submission requirements are listed herein.  These values
# need to be changed to True (instead of None).
sanity_checks = {
	
	# For the TokenCC that you are using for this assignment, did you make the
	# changes required in the DEX homework?  This is adding the
	# `_afterTokenTransfer()` function.
	'modified_tokencc': True,

	# Did you register your DEX with the course dex.php web page? This implies
	# that you deployed both TokenDEX and TokenCC to the private Ethereum
	# blockchain.
	'registered_dex_with_course_page': True,

	# Did you call createPool() on your DEX with *exactly* 100 (fake) ETH? 
	'called_createpoool_with_100_eth': True,

	# When you called createPool(), did you send in at least 10.0 of your TC?
	# You can use more, if you would like.
	'called_createpoool_with_10_tc': True,

	# Is your DEX initialized with the *variable* EtherPricer contract?
	'initialized_dex_with_variable_etherpriceoracle': True,

	# Did you send me exactly 10.0 of your token cryptocurrencty?  If your
	# token cryptocurrency uses 8 decimals, then that will be 1,000,000,000
	# total units sent.  This is from the TokenCC that you deployed in
	# the 'contracts' section, above.
	'sent_me_10_of_your_cc': True,

	# Did you, or will you, make 4 exchanges on somebody else's DEX?  These
	# bids are due 24 hours after the assignment due date
	'made_4_exchanges_on_another_dex': True,

}


# While some of these are optional, you still have to replace those optional
# ones with the empty string (rather than None).
comments = {

	# How long did this assignment take, in hours?  Please format as an
	# integer or float.
	'time_taken': 7.75,

	# Any suggestions for how to improve this assignment?  This part is
	# completely optional.  If none, then you can have the value here be the
	# empty string (but not None).
	'suggestions': "I did not understand how to use the testing file and did not use it",

	# Any other comments or feedback?  This part is completely optional. If
	# none, then you can have the value here be the empty string (but not
	# None).
	'comments': "testing for this assignment was very tedious. redeploy, test 1 function, redeploy, ...",
}
