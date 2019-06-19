#include <iostream>
#include <string>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/tl/formula.hh>
#include <spot/tl/simplify.hh>
#include "olg_check.h"

spot::formula simplify(const spot::formula &f)
{
  static spot::tl_simplifier_options options(
		  false,true,true,false,false,false,false,false,true);
  static spot::tl_simplifier simplifier(options);
  return simplifier.simplify(f);
}


int main()
{
	std::string input;
	puts("please input the formula:");
	getline(std::cin,input);
	spot::formula LTLformula=spot::parse_formula(input);

	//spot::tl_simplifier simp;
	LTLformula=simplify(LTLformula);
	print_psl(std::cout,LTLformula)<<"  --origin\n";
	
	olg_check ch(LTLformula);
	printf ("%s\n", ch.is_sat () ? "sat" : "unsat");	
	std::cout<<ch._evidence<<"\n";
	//bool is_sat=ch.check(); std::cout<<"5\n";
	/*if(is_sat) 
		std::cout<<"sat"<<"\n";
	else
		std::cout<<"unsat"<<"\n";*/
	
	return 0;
}

