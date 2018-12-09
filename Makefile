calc: main.c
	gcc -std=gnu11 -o jitcalc main.c
jitcalc: main.c
	gcc -std=gnu11 -o jitcalc -D JIT main.c

normal-test: calc
	sh test.sh
jit-test: jitcalc
	sh test.sh
test: normal-test clean jit-test ;

clean:
	-rm -f jitcalc jitcalc.exe