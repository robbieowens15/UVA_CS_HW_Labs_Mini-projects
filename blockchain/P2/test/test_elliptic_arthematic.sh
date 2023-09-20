#!/bin/bash
cd '/Users/robbieowens/Documents/Documents/SchoolWork/College/7th Semester/Crypto/P2'
echo "ecdsa.sh elp_add 12 12 42 36 43"
./ecdsa.sh elp_add 12 12 42 36 43
echo "___________________________"
echo "(12, 31)"
echo "*************************************"
echo "ecdsa.sh elp_add 7 7 7 7 43"
./ecdsa.sh elp_add 7 7 7 7 43
echo "___________________________"
echo "(21, 18)"
echo "*************************************"
echo "ecdsa.sh elp_add 2 12 42 7 43"
./ecdsa.sh elp_add 2 12 42 7 43
echo "___________________________"
echo "(40, 25)"
echo "*************************************"
echo "ecdsa.sh elp_add_self 2 12 43"
./ecdsa.sh elp_add_self 2 12 43
echo "___________________________"
echo "(7, 7)"
echo "*************************************"
echo "ecdsa.sh elp_add_self 34 3 43"
./ecdsa.sh elp_add_self 34 3 43
echo "___________________________"
echo "(35, 21)"
echo "*************************************"
echo "ecdsa.sh elp_k_mul_self 34 3 5 43"
./ecdsa.sh elp_k_mul_self 34 3 5 43
echo "___________________________"
echo "(32, 3)"
echo "*************************************"
echo "ecdsa.sh elp_k_mul_self 2 31 13 43"
./ecdsa.sh elp_k_mul_self 2 31 13 43
echo "___________________________"
echo "(13, 22)"
echo "*************************************"
echo "ecdsa.sh elp_k_mul_self 7 36 64 43"
./ecdsa.sh elp_k_mul_self 7 36 64 43
echo "___________________________"
echo "(21, 25)"
echo "*************************************"
echo "ecdsa.sh elp_k_mul_self 2 31 1 43"
./ecdsa.sh elp_k_mul_self 2 31 1 43
echo "___________________________"
echo "(2, 31)"
echo "*************************************"
echo "ecdsa.sh elp_k_mul_self 2 31 2 43"
./ecdsa.sh elp_k_mul_self 2 31 2 43
echo "___________________________"
echo "(7, 36)"
echo "*************************************"
echo "ecdsa.sh elp_k_mul_self 2 31 3 43"
./ecdsa.sh elp_k_mul_self 2 31 3 43
echo "___________________________"
echo "(35, 22)"