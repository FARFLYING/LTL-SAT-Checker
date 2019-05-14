#ifndef OLG_FORMULA
#define OLG_FORMULA

#include <iostream>
#include <spot/tl/print.hh>
#include <spot/tl/formula.hh>

class olg_formula{
	private:
		//spot::formula _left;
		//spot::formula _right;
		spot::formula olgFormula;
	public:
		spot::op top_most;
		olg_formula(spot::formula);
		//olg_formula();
		~olg_formula();
		spot::formula build_olg_formula(spot::formula);
		std::vector<std::string> split_formula(spot::formula);
		spot::formula convert_to_CNF(spot::formula);
		void get_all_apName(spot::formula);
		void write_dimacs(spot::formula);
		bool sat();
		

};

#endif