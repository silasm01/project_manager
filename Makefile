CC = gcc                # Compiler
CFLAGS = -Wall -g       # Compiler flags: -Wall for warnings, -g for debugging
AR = ar                 # Archiver for static libraries
LIBS = -L. -llinked # Link hashmap and linked libraries
EXEC = main

$(EXEC): $(EXEC).c $(TARGET)
	$(CC) $(CFLAGS) -o proj $(EXEC).c $(LIBS)

clean:
	rm -f $(EXEC) $(EXEC).o