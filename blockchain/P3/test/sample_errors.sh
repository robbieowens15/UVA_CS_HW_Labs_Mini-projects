#!/bin/bash
cd /Users/robbieowens/Documents/Documents/SchoolWork/College/7th\ Semester/Crypto/P3

echo "expected e1 b0"
cat blk00000-f10.blk | ./change_byte.py 2 0 > test.blk
./parse.sh test.blk
echo "expected e2 b0"
cat blk00000-f10.blk | ./change_byte.py 10 7 > test.blk
./parse.sh test.blk
echo "expected e3 b1"
cat blk00000-f10.blk | ./change_byte.py 14 3 > test.blk
./parse.sh test.blk
echo "expected e3 b1"
cat blk00000-f10.blk | ./change_byte.py 310 23 > test.blk
./parse.sh test.blk
echo "expected e4 b1"
cat blk00000-f10.blk | ./change_byte.py 372 0 > test.blk
./parse.sh test.blk
echo "expected e5 b0"
cat blk00000-f10.blk | ./change_byte.py 89 17 > test.blk
./parse.sh test.blk
echo "expected e6 b0"
cat blk00000-f10.blk | ./change_byte.py 46 3 > test.blk
./parse.sh test.blk

rm test.blk