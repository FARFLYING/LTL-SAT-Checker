#include "olg_check.h"
#include <iostream>
#include <spot/tl/formula.hh>
#include "olg_formula.h"
#include "dnf_formula.h"



olg_check::olg_check(spot::formula f){
	formula_check=clone(f);
}

olg_check::~olg_check(){
	//formula_check.~formula();
}

spot::formula olg_check::clone(spot::formula in){
	return in;
}

bool olg_check::check(){
	if(formula_check.is_tt())
		return true;
	else if(formula_check.is_ff())
		return false;
	else{
       /* olg_formula olg(formula_check);
		//bool is_olg_sat=olg.sat(); std::cout<<"4 "<<is_olg_sat<<"\n";
		if(olg.sat())
			return true;*/
		
		dnf_formula *dnf = new dnf_formula(formula_check);
		std::cout<<(*dnf).to_string().c_str()<<"\n";
	}
	return false;
}
