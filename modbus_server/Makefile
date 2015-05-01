
FLAGS = -I /usr/include/modbus

LDIR = /usr/lib
LIBS = -lmodbus -pthread


all: main.c modbus_master.c
	gcc $(FLAGS) *.c -L$(LDIR) $(LIBS) -o server

clean:
	rm *o server