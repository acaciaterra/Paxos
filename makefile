all: paxos

paxos: paxos.o smpl.o rand.o
	$(LINK.cpp) -o $@ -Bstatic paxos.o smpl.o rand.o -lm

smpl.o: smpl.cpp smpl.h
	$(COMPILE.cpp)  -g smpl.cpp

paxos.o: paxos.cpp smpl.h
	$(COMPILE.cpp) -g  paxos.cpp -std=c++17

rand.o: rand.c
	$(COMPILE.c) -g rand.c

clean:
	$(RM) *.o paxos relat saida
