src= $(wildcard *.c)
obj= $(patsubst *.c,*.o,$(src))

all:FORCE
	gcc -o $@ $(obj) 
FORCE:
