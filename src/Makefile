CC      = gcc -std=c99
C_FLAGS = -Wall -g3 -ggdb3 -pedantic -fgnu89-inline `mysql_config --cflags`
L_FLAGS = `mysql_config --libs` -lz -lpthread -lcrypt -llua -ldl -lm

O_FILES = socket.o io.o strings.o utils.o interpret.o help.o  \
	  action_safe.o mccp.o save.o event.o event-handler.o \
	  list.o stack.o buffers.o lua_utils.o account.o \
          lua_account.o muddb.o id_handler.o nanny.o lua_nanny.o \
          vars.o lua_socket.o

all: $(O_FILES)
	rm -f DavEngine
	$(CC) -o ../DavEngine $(O_FILES) $(L_FLAGS)

.c.o: all
	@echo [`date +%T`] Compiling $< ...
	@$(CC) -c $(C_FLAGS) $<

clean:
	@echo Cleaning code $< ...
	@rm -f *.o
	@rm -f ../DavEngine
	@rm -f *.*~
