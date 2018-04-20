all : maxcut

maxcut : maxcut.o
	g++ -O3 -o maxcut maxcut.o

maxcut.o : maxcut.cpp maxcut.h
	g++ -O3 -c -o maxcut.o maxcut.cpp

clean :
	rm -rf *.o maxcut
