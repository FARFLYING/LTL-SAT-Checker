LST:main.o olg_check.o olg_formula.o
	g++ main.o olg_check.o olg_formula.o -o LST -lspot
	-rm -f *.o
main.o:main.cpp
	g++ -c main.cpp -std=c++11 -lspot
olg_check.o:olg_check.cpp
	g++ -c olg_check.cpp -std=c++11 -lspot
olg_formula.o:olg_formula.cpp
	g++ -c olg_formula.cpp -std=c++11 -lspot
.PHONY: clean
clean:
	-rm -f *.o
