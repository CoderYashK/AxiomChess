program: attack.o axiom.o bitboards.o board.o data.o evaluate.o hashkeys.o init.o io.o makemove.o misc.o movegen.o perft.o polybook.o polykeys.o pvtable.o search.o uci.o validate.o xboard.o
	cc -g attack.o bitboards.o board.o data.o evaluate.o hashkeys.o init.o io.o  makemove.o  misc.o movegen.o  perft.o polybook.o polykeys.o pvtable.o search.o uci.o validate.o axiom.o xboard.o -o program 

attack.o: attack.c defs.h
	cc -g -c attack.c

axiom.o: axiom.c defs.h
	cc -g -c axiom.c

bitboards.o: bitboards.c defs.h
	cc -g -c bitboards.c

board.o: board.c defs.h
	cc -g -c board.c

data.o: data.c defs.h
	cc -g -c data.c

evaluate.o: evaluate.c defs.h
	cc -g -c evaluate.c

hashkeys.o: hashkeys.c defs.h
	cc -g -c hashkeys.c

init.o: init.c defs.h
	cc -g -c init.c

io.o: io.c defs.h
	cc -g -c io.c

makemove.o: makemove.c defs.h
	cc -g -c makemove.c

misc.o: misc.c defs.h
	cc -g -c misc.c

movegen.o: movegen.c defs.h
	cc -g -c movegen.c

perft.o: perft.c defs.h
	cc -g -c perft.c

polybook.o: polybook.c defs.h polykeys.h
	cc -g -c polybook.c

polykeys.o: polykeys.c defs.h polykeys.h
	cc -g -c polykeys.c

pvtable.o: pvtable.c defs.h
	cc -g -c pvtable.c

search.o: search.c defs.h
	cc -g -c search.c

uci.o: uci.c defs.h
	cc -g -c uci.c

validate.o: validate.c defs.h
	cc -g -c validate.c 

xboard.o: xboard.c defs.h
	cc -g -c xboard.c 

clean:
	rm *.o






















































