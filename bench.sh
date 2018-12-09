echo "calc (normal)"
make calc
time ./jitcalc < fib.calc

echo "calc (jit)"
make jitcalc
time ./jitcalc < fib.calc