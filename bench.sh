echo "fib . 38 (normal)"
make calc
time ./jitcalc < fib.calc
make clean
echo "fib . 38 (jit)"
make jitcalc
time ./jitcalc < fib.calc