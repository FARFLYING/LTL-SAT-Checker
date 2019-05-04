LST:main.o olg_check.o
	g++ main.o olg_check.o -o LST -lspot
main.o:main.cpp
	g++ -c main.cpp -std=c++11 -lspot
olg_check.o:olg_check.cpp
	g++ -c olg_check.cpp -std=c++11 -lspot
.PHONY clean:
	-rm -f *.o