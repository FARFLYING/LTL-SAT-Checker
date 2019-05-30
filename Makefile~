LST:main.o olg_check.o olg_formula.o dnf_formula.o dnf_clause.o 
	g++ main.o olg_check.o olg_formula.o dnf_formula.o dnf_clause.o -o LST -lspot
	-rm -f *.o
main.o:main.cpp
	g++ -c main.cpp -std=c++11 -lspot
olg_check.o:olg_check.cpp
	g++ -c olg_check.cpp -std=c++11 -lspot
olg_formula.o:olg_formula.cpp
	g++ -c olg_formula.cpp -std=c++11 -lspot
dnf_formula.o:dnf_formula.cpp
	g++ -c dnf_formula.cpp -std=c++11 -lspot
dnf_clause.o:dnf_clause.cpp
	g++ -c dnf_clause.cpp -std=c++11 -lspot
.PHONY: clean
clean:
	-rm -f *.o
