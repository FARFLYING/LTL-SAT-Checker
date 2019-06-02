#include <iostream>
#include <string>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/tl/formula.hh>
#include <spot/tl/simplify.hh>
#include "olg_check.h"

int main()
{
	std::string input;
	puts("please input the formula:");
	getline(std::cin,input);
	spot::formula LTLformula=spot::parse_formula(input);

	spot::tl_simplifier simp;
	LTLformula=simp.simplify(LTLformula);
	print_psl(std::cout,LTLformula)<<"  --origin\n";
	
	olg_check ch(LTLformula);
	printf ("%s\n", ch.is_sat () ? "sat" : "unsat");	
	std::cout<<ch._evidence<<"\n";
	/*bool is_sat=ch.check(); std::cout<<"5\n";
	if(is_sat) 
		std::cout<<"sat"<<"\n";
	else
		std::cout<<"unsat"<<"\n";*/
	
	return 0;
}


