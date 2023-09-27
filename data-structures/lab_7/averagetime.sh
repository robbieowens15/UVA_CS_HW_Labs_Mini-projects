#!/bin/bash
SUM=0
NUM_TIMES_TO_RUN=5
echo "enter the exponent for counter.cpp:"
read EXP
if [ "$EXP" == "quit" ] ; then
	echo "Exiting Script"
	exit 0
fi
for i in  {1..5} ; do
	echo "Running iteration $i"
	TEMP=`./a.out $EXP | tail -1`
	echo "Time taken: $TEMP ms"
	SUM=$(( $TEMP + $SUM ))	
done
echo "5 interations took $SUM ms"
AVERAGE=$(( SUM / 5 ))
echo "Average time was $AVERAGE ms"
