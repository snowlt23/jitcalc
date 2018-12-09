#!/bin/sh

# input=$1, result=$2
runtest() {
  OUT=`echo "$1" | ./jitcalc`
  OUT=`echo $OUT`
  if [ "$OUT" != "$2" ] ; then
    echo "[ERROR] $1: expect $2, but got $OUT"
    exit 1
  fi
}

runtest "5" "5"
runtest "4 + 5" "9"
runtest "f = 555" "0"
runtest "f = 555; 5" "0 5"
runtest "f = 555; f . 0" "0 555"
runtest "f = .; f . 45" "0 45"