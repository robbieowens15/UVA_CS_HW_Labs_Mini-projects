#!/bin/bash
echo "ecdsa.sh add 13 25 17"
cd '/Users/robbieowens/Documents/Documents/SchoolWork/College/7th Semester/Crypto/P2'
./ecdsa.sh add 13 25 17
echo "___________________________"
echo 4
echo "*************************************"
echo "ecdsa.sh mul 6 6 17"
./ecdsa.sh mul 6 6 17
echo "___________________________"
echo 2
echo "*************************************"
echo "ecdsa.sh exp 3 3 17"
./ecdsa.sh exp 3 3 17
echo "___________________________"
echo 10
echo "*************************************"
echo "ecdsa.sh add_inv 10 17"
./ecdsa.sh add_inv 10 17
echo "___________________________"
echo 7
echo "*************************************"
echo "Identity: 15 - 10 = 5 =? " `./ecdsa.sh add 15 7 17` "(./ecdsa.sh add 15 7 17)"
echo "*************************************"
echo "ecdsa.sh sub 15 10 17"
./ecdsa.sh sub 15 10 7
echo "___________________________"
echo 5
echo "*************************************"
echo "ecdsa.sh sub -6 17 17"
./ecdsa.sh sub -6 17 17
echo "___________________________"
echo 11
echo "*************************************"
echo "ecdsa.sh mul_inv 13 17"
./ecdsa.sh mul_inv 13 17
echo "___________________________"
echo 4
echo "*************************************"
echo "ecdsa.sh div 13 5 17"
./ecdsa.sh div 13 5 17
echo "___________________________"
echo 6
