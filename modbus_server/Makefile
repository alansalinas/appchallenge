
FLAGS = -I /usr/include/modbus

LDIR = /usr/lib
LIBS = -lmodbus


all: main.c modbus_master.c
	gcc $(FLAGS) main.c modbus_master.c -L$(LDIR) $(LIBS) -o server

clean:
	rm *o server