all : maxcut

maxcut : maxcut.o
	g++ -o maxcut maxcut.o

maxcut.o : maxcut.cpp maxcut.h
	g++ -c -o maxcut.o maxcut.cpp

clean :
	rm -rf *.o maxcut
