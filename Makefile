jitcalc: main.c
	gcc -std=gnu11 -o jitcalc main.c

test: jitcalc
	sh test.sh

clean:
	rm jitcalc jitcalc.exe