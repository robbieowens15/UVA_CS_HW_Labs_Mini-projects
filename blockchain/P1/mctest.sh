#!/bin/bash

./cryptomoney.sh genesis
# Wallet for User 1
./cryptomoney.sh generate user1wallet.txt
export user1=`./cryptomoney.sh address user1wallet.txt`
# Wallet for User 2
./cryptomoney.sh generate user2wallet.txt
export user2=`./cryptomoney.sh address user2wallet.txt`

# Fund wallets with 10 each
./cryptomoney.sh fund $user1 10 01-user1-funding.txt
./cryptomoney.sh fund $user2 10 02-user2-funding.txt

echo ---Transfer 1----
# Successful Transfer of 5 to User 2
./cryptomoney.sh transfer user1wallet.txt $user2 5 03-user1-to-user2.txt
echo ---Transfer 2----
# User 1 needs some cash tho so 6 back
./cryptomoney.sh transfer user2wallet.txt $user1 6 04-user2-to-user1.txt

# Verify
./cryptomoney.sh verify user1wallet.txt 01-user1-funding.txt
./cryptomoney.sh verify user2wallet.txt 02-user2-funding.txt
./cryptomoney.sh verify user1wallet.txt 03-user1-to-user2.txt
./cryptomoney.sh verify user2wallet.txt 04-user2-to-user1.txt

# Check Mempool
echo -----Mempool----
cat mempool.txt
echo -----End Mempool----

echo ---Balances of 11 and 9----
./cryptomoney.sh balance $user1
./cryptomoney.sh balance $user2
echo ----End Balances----

# Mine first block
./cryptomoney.sh mine 4

echo ----Sha----
sha256sum block_1.txt
echo ---End Sha----

# Validate 
./cryptomoney.sh validate

# Try failed transfer
./cryptomoney.sh transfer user1wallet.txt $user2 200 05-fail-user1-user2.txt

./cryptomoney.sh verify user1wallet.txt 05-fail-user1-user2.txt

# Check mempool
echo -----Mempool----
# Check Mempool
cat mempool.txt
echo -----End Mempool----

# Go to zero money
./cryptomoney.sh transfer user1wallet.txt $user2 11 06-runoutmoney-user1-user2.txt

./cryptomoney.sh verify user1wallet.txt 06-runoutmoney-user1-user2.txt

echo ---Balances of 0 and 20----
./cryptomoney.sh balance $user1
./cryptomoney.sh balance $user2
echo ----End Balances----

echo -----Mempool----
# Check Mempool
cat mempool.txt
echo -----End Mempool----

./cryptomoney.sh mine 1

echo ---Balances of 0 and 20----
./cryptomoney.sh balance $user1
./cryptomoney.sh balance $user2
echo ----End Balances----