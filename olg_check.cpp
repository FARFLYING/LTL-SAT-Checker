#include "olg_check.h"
#include <iostream>
#include <spot/tl/formula.hh>
#include "olg_formula.h"
#include "dnf_formula.h"
#include <vector>



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
		
		//print_psl(std::cout,trans_F_G(formula_check))<<"\n";
		dnf_formula *dnf = new dnf_formula(trans_F_G(formula_check));
		std::cout<<(*dnf).to_string().c_str()<<"\n";
	}
	return false;
}

spot::formula olg_check::trans_F_G(spot::formula to_trans){
	if(to_trans.is_constant() | to_trans.is_literal()){
		return to_trans;
	}
	else if(to_trans.kind()==spot::op::G){//G a = ff R a
		return spot::formula::R(spot::formula::ff(),trans_F_G(to_trans.operator[](0)));
	}
	else if(to_trans.kind()==spot::op::F){//F a = tt U a
		return spot::formula::U(spot::formula::tt(),trans_F_G(to_trans.operator[](0)));	
	}
	else if(to_trans.kind()==spot::op::R){
		return spot::formula::R(trans_F_G(to_trans.operator[](0)),trans_F_G(to_trans.operator[](1)));	
	}
	else if(to_trans.kind()==spot::op::U){
		return spot::formula::U(trans_F_G(to_trans.operator[](0)),trans_F_G(to_trans.operator[](1)));	
	}
	else if(to_trans.kind()==spot::op::Or){
		std::vector<spot::formula> temp;
		for(int i=0;i<to_trans.size();i++){
			temp.push_back(trans_F_G(to_trans.operator[](i)));		
		}
		return spot::formula::Or(temp);
	}
	else if(to_trans.kind()==spot::op::And){
		std::vector<spot::formula> temp;
		for(int i=0;i<to_trans.size();i++){
			temp.push_back(trans_F_G(to_trans.operator[](i)));		
		}
		return spot::formula::And(temp);
	}
}
