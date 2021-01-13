CC=clang -Wall -DLINUX

# use for windows binary compile
WCC=i686-w64-mingw32-gcc --static -DWINDOWS

CFLAGS = -lm
DEBUG = -g -DDEBUG

all: tool.o tdtoolkit.o src/main.c
	$(CC) $(CFLAGS) -o bin/tdtoolkit tool.o tdtoolkit.o src/main.c

tdtoolkit.o: src/tdtoolkit.h src/tdtoolkit.c
	$(CC) -c src/tdtoolkit.c -o tdtoolkit.o

tool.o: src/tool.h src/tool.c
	$(CC) -c src/tool.c -o tool.o 

debug: tdtoolkit_dbg.o src/main.c tool_dbg.o
	$(CC) $(DEBUG) $(CFLAGS) -o bin/tdtoolkit_dbg tdtoolkit_dbg.o tool_dbg.o src/main.c

tdtoolkit_dbg.o: src/tdtoolkit.c src/tdtoolkit.h
	$(CC) $(DEBUG) -c src/tdtoolkit.c -o tdtoolkit_dbg.o

tool_dbg.o: src/tool.h src/tool.c
	$(CC) $(DEBUG) -c src/tool.c -o tool_dbg.o

Windows: tdtoolkit_win.o tool_win.o src/main.c
	$(WCC) $(CFLAGS) -o bin/tdtoolkit_win.exe tool_win.o tdtoolkit_win.o src/main.c

tdtoolkit_win.o: src/tdtoolkit.h src/tdtoolkit.c
	$(WCC) -c src/tdtoolkit.c -o tdtoolkit_win.o

tool_win.o: src/tool.h src/tool.c
	$(WCC) -c src/tool.c -o tool_win.o 

test: src/test.c tool.o tdtoolkit.o
	$(CC) $(CFLAGS) -o test tool.o tdtoolkit.o src/test.c

testWindows: src/test_input.c
	$(WCC) -o bin/test_input.exe src/test_input.c

clean:
	rm -rf bin/* *.o
