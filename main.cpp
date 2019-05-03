//#include <stdio.h>
#include <string>
#include <iostream>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/tl/formula.hh>
//using namespace std;

std::string input;

int main()
{
	puts("please input the formula:");
	getline(std::cin,input);
	spot::formula LTLformula=spot::parse_formula(input);
	//input = "GFp0 | FGp1";
  	//spot::parsed_formula pf = spot::parse_infix_psl(input);
	//std::cout << pf.f << '\n';
	print_psl(std::cout,LTLformula)<<"\n";

	return 0;
}


