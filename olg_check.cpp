#include "olg_check.h"
#include <iostream>
#include <spot/tl/formula.hh>
#include "olg_formula.h"

olg_check::olg_check(spot::formula f){
	formula_check=f;
}

olg_check::~olg_check(){
	formula_check.~formula();
}

bool olg_check::check(){
	if(formula_check.is_tt())
		return true;
	else if(formula_check.is_ff())
		return false;
	else{
        olg_formula olg(formula_check);
	}
}
