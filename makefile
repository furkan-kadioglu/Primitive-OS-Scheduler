thread : main.o
	g++ -o thread main.o
main.o : main.cpp
	g++ -c main.cpp
clean : 
	rm *.o
