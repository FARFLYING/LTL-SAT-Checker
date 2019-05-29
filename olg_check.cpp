#include "olg_check.h"
#include <iostream>
#include <spot/tl/formula.hh>
#include "olg_formula.h"
#include "dnf_formula.h"
#include <vector>

olg_check::timestamp olg_check::dfn;
olg_check::timestamp olg_check::low;

std::vector<olg_check::edge_set> olg_check::_path;
std::vector<spot::formula *> olg_check::_states;

olg_check::olg_check(spot::formula f){
	formula_check=clone(f);
}

olg_check::~olg_check(){
	//formula_check.~formula();
}

spot::formula olg_check::clone(spot::formula in){
	return in;
}

void olg_check::init ()
{
  this->_index = 1;
  this->low.clear ();
  this->dfn.clear ();
  while (!this->_stk.empty ())this->_stk.pop ();
  this->_instk.clear ();
  this->_result = "";
  _path.clear();
  _states.clear();
  _evidence = "";
}

bool olg_check::is_sat(){
	init();
	return check();
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
		
		spot::formula classified=trans_F_G(formula_check);
		print_psl(std::cout,classified)<<" --classify\n";
		dnf_formula *dnf = new dnf_formula(classified);
		//std::cout<<(*dnf).to_string().c_str()<<"\n";

		edge_set *es = new edge_set ();
  		_scc[dnf] = es;
  		edge_set::iterator eit;

		_stk.push (*classified);
  		_instk.insert (*classified);
		
		spot::formula *v;
		dnf_formula::dnf_clause_set *dc_set = dnf->get_next ();

		int size = dc_set->size ();

		if (size > 0){
			int i = 0;
      		dnf_clause *dnf_arr = new dnf_clause[size + 1];

			for (dnf_formula::dnf_clause_set::iterator it = dc_set->begin (); it != dc_set->end (); it++)
      		{
        		dnf_arr[i++] = *it;
		
        		//printf("%s\n", (*it).to_string().c_str());
      		}
		}
	}
	return false;
}

spot::formula olg_check::trans_F_G(spot::formula to_trans){
	if(to_trans.kind()==spot::op::ap){
		return to_trans;
	}
	else if(to_trans.kind()==spot::op::Not){
		return spot::formula::Not(trans_F_G(to_trans.operator[](0)));	
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
	else if(to_trans.kind()==spot::op::X){
		return  spot::formula::X(trans_F_G(to_trans.operator[](0)));	
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
