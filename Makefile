all: clean	
	g++ -I. -std=c++11 -O3 -c main.cc -o main.o
	g++ main.o -o ring_buffer

run: all	
	time ./ring_buffer

clean:
	rm -f *.o ring_buffer