#!/bin/bash
./cryptomoney.sh name
./cryptomoney.sh genesis
./cryptomoney.sh generate alice.wallet.txt
export alice=`./cryptomoney.sh address alice.wallet.txt`
echo alice.wallet.txt wallet signature: $alice
./cryptomoney.sh fund $alice 100 01-alice-funding.txt
./cryptomoney.sh generate bob.wallet.txt
export bob=`./cryptomoney.sh address bob.wallet.txt`
echo bob.wallet.txt wallet signature: $bob
./cryptomoney.sh fund $bob 100 02-bob-funding.txt
./cryptomoney.sh verify alice.wallet.txt 01-alice-funding.txt
./cryptomoney.sh verify bob.wallet.txt 02-bob-funding.txt
cat mempool.txt
./cryptomoney.sh mine 1
sha256sum block_1.txt
./cryptomoney.sh validate