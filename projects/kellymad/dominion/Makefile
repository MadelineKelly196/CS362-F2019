CFLAGS= -Wall -fpic -coverage -lm -std=c99

rngs.o: rngs.h rngs.c
	gcc -c rngs.c -g  $(CFLAGS)

dominion.o: dominion.h dominion.c rngs.o
	gcc -c dominion.c -g  $(CFLAGS)

playdom: dominion.o playdom.c
	gcc -o playdom playdom.c -g dominion.o rngs.o $(CFLAGS)
#To run playdom you need to entere: ./playdom <any integer number> like ./playdom 10*/

cardtest4: cardtest4.c dominion.c rngs.c
	gcc -o cardtest4 -g  cardtest4.c dominion.c rngs.c $(CFLAGS)

testDrawCard: testDrawCard.c dominion.o rngs.o
	gcc  -o testDrawCard -g  testDrawCard.c dominion.o rngs.o $(CFLAGS)

interface.o: interface.h interface.c
	gcc -c interface.c -g  $(CFLAGS)

runtests: testDrawCard
	./testDrawCard &> unittestresult.out
	gcov dominion.c >> unittestresult.out
	cat dominion.c.gcov >> unittestresult.out

unittest1: unittest1.c dominion.c rngs.c
	gcc -o unittest1 -g  unittest1.c dominion.c rngs.c $(CFLAGS)

unittest2: unittest2.c dominion.c rngs.c
	gcc -o unittest2 -g  unittest2.c dominion.c rngs.c $(CFLAGS)

unittest4: unittest4.c dominion.c rngs.c
	gcc -o unittest4 -g  unittest4.c dominion.c rngs.c $(CFLAGS)

unittestresults: unittest1 unittest2unittest4
	./unittest1 &> unittestresults.out
	gcov dominion.c -b -f >> unittestresults.out
	cat dominion.c.gcov >> unittestresults.out
	./unittest2 &> unittestresults.out
	gcov dominion.c -b -f >> unittestresults.out
	cat dominion.c.gcov >> unittestresults.out
	./unittest4 &> unittestresults.out
	gcov dominion.c -b -f >> unittestresults.out
	cat dominion.c.gcov >> unittestresults.out

player: player.c interface.o
	gcc -o player player.c -g  dominion.o rngs.o interface.o $(CFLAGS)

all: playdom player

clean:
	rm -f *.o playdom.exe playdom player player.exe  *.gcov *.gcda *.gcno *.so *.out testDrawCard testDrawCard.exe
