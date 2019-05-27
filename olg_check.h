#ifndef OLG_CHECK
#define OLG_CHECK

#include <iostream>
#include <spot/tl/print.hh>
#include <spot/tl/formula.hh>

class olg_check{
	public:
		olg_check(spot::formula);
		~olg_check();
		bool check();
		spot::formula clone(spot::formula);
	private:
		spot::formula formula_check;
};

#endif
