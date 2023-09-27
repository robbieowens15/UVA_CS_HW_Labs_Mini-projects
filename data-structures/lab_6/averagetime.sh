#!/bin/bash
read DICTONARY
read GRID
RUN_TIME_ONE=`./a.out $DICTONARY $GRID | tail -1`
RUN_TIME_TWO=`./a.out $DICTONARY $GRID | tail -1`
RUN_TIME_THREE=`./a.out $DICTONARY $GRID | tail -1`
RUN_TIME_FOUR=`./a.out $DICTONARY $GRID | tail -1`
RUN_TIME_FIVE=`./a.out $DICTONARY $GRID | tail -1`
SUM=`echo $RUN_TIME_ONE + $RUN_TIME_TWO + $RUN_TIME_THREE + $RUN_TIME_FOUR + $RUN_TIME_FIVE | bc`
AVERAGE=`echo "scale = 6; $SUM / 5.0" | bc`
echo $AVERAGE
