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
runtest "1 < 5" "1"
runtest "2 < 1" "0"
runtest "(1 < 2) + 1" "2"
runtest "1 < (2 + 1)" "1"
runtest "f = 555" "0"
runtest "f = 555; 5" "0 5"
runtest "f = 555; f . 0" "0 555"
runtest "f = .; f . 45" "0 45"
runtest "if 1; 4; 5;" "4"
runtest "if 0; 4; 5;" "5"