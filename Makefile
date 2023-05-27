CC=gcc
CFlags=-Wall
LDFLAGS=-lm
EXE=allocate

$(EXE): main.c scheduler.c scheduler.h queue.c queue.h
	$(CC) $(CFlags) -o $(EXE) main.c scheduler.c scheduler.h queue.c queue.h $(LDFLAGS)

clean: 
	rm -f *.o $(EXE)