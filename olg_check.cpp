#include "olg_check.h"
#include <iostream>
#include <spot/tl/formula.hh>
#include "olg_formula.h"
#include "dnf_formula.h"
#include <vector>
#include <spot/tl/print.hh>
#include <spot/tl/parse.hh>
#include <string>
#include <fstream>

olg_check::timestamp olg_check::dfn;
olg_check::timestamp olg_check::low;

std::vector<olg_check::edge_set> olg_check::_path;
std::vector<spot::formula *> olg_check::_states;

std::vector<spot::formula *> olg_check::all_formula;

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
	spot::formula *classified=new spot::formula(trans_F_G(formula_check));
	//print_psl(std::cout,*classified)<<" --classify\n";
	return check(classified);
}

bool olg_check::check(spot::formula *u){

	//spot::formula classified=trans_F_G(*u);  //std::cout<<"step c\n";
	//spot::formula *u=new spot::formula(classified);   //std::cout<<"step d\n";
	//spot::formula pre_u=spot::formula(*u);

	dfn[u] = low[u] = _index++; //print_psl(std::cout,*u)<<" --u "<<_index<<"\n";
	
	if(u->is_tt()){
		_evidence = "true";
		return true;
	}
	else if(u->is_ff())
		return false;
	else{
        olg_formula olg(*u);
		//bool is_olg_sat=olg.sat(); std::cout<<"4 "<<is_olg_sat<<"\n";
		if(olg.sat()){
			get_evidence();
			return true;
		}
		
		
		dnf_formula *dnf = dnf_formula(u).unique ();   // std::cout<<"step a\n";
		//std::cout<<(*dnf).to_string().c_str()<<"\n";

		edge_set *es = new edge_set ();
  		_scc[dnf] = es;
  		edge_set::iterator eit;

		_stk.push (u);
  		_instk.insert (u);
		
		spot::formula *v;
		dnf_formula::dnf_clause_set *dc_set = dnf->get_next ();  //std::cout<<"step b\n";

		int size = dc_set->size ();

		if (size > 0){
			int i = 0;
      		dnf_clause *dnf_arr = new dnf_clause[size + 1];
			//printf("dnf_clause:\n");
			for (dnf_formula::dnf_clause_set::iterator it = dc_set->begin (); it != dc_set->end (); it++)
      		{
        		dnf_arr[i++] = *it;
		
        		//printf("%s\n", (*it).to_string().c_str());
      		}
			std::sort (dnf_arr, dnf_arr + size);
		
			for (i = 0; i < size; ++i){
				_path.push_back(and_to_set(dnf_arr[i].current));
				_states.push_back(u);

				v = unique(dnf_arr[i].next);  
				//spot::formula pre_v=spot::formula(*v);
				//std::cout<<spot::str_psl(*v)<<" --v\n";

				if (dfn.find (v) == dfn.end ()){
					//std::cout<<"step 2\n";
					//spot::formula temp=spot::parse_formula(spot::str_psl(*v));
					if (check(v)) return true;
					//std::cout<<"step 2.5\n";
					timestamp::iterator u_it = low.find (u);
              		timestamp::iterator v_it = low.find (v);
					if (u_it->second > v_it->second)
                		u_it->second = v_it->second;
					//std::cout<<"step 3\n";
				}
				else if (_instk.find (v) != _instk.end ()){
					timestamp::iterator u_it = low.find (u); //cout<<u_it->second<<" --4 u_it->second\n";
              		timestamp::iterator v_it = low.find (v); //cout<<v_it->second<<" --4 v_it->second\n";
              		if (u_it->second > v_it->second)
               	 		u_it->second = v_it->second;
				}

				if (low[u] == low[v]){
					split2set (spot::op::And, dnf_arr[i].current, es);
					if (u != v){
						edge_set *next_edge = _scc[dnf_formula::get_dnf (v)];
						for (eit = next_edge->begin (); eit != next_edge->end (); eit++)
							es->insert (*eit);
					}
					if(scc_sat (unique(u), es)){
						spot::formula *aff = _states.back();
						_states.pop_back();
						std::vector<edge_set> path;
						while(aff != v)
                  		{
                    		path.push_back(_path.back());
                    		_path.pop_back();
                  		}
						generate_evidence(_path, path);
						delete[] dnf_arr;
                  		return true;
					}
				}
			}
			delete[] dnf_arr;
		}
		
		//spot::formula pre_v2=spot::formula(*v);
		if (dfn[u] == low[u]){
			do{
				v = _stk.top ();
          		_instk.erase (v);
          		_stk.pop ();
        	}while (v != u);
		//std::cout<<"step 8\n";
		}
		return false;
	}
	return false;
}

void olg_check::split2set(spot::op op,spot::formula *f,edge_set *s){
	if (f == NULL) return;

	if(f->kind()!=op){
		//std::cout<<"split2set 1\n";
		s->insert (unique(f));}
	else{
		//std::cout<<"split2set 2\n";
		spot::formula *left=new spot::formula(f->all_but(f->size()-1));
		spot::formula *right=new spot::formula(f->operator[](f->size()-1));
		split2set(op,unique(left),s);
		split2set(op,unique(right),s);
	}
}

void olg_check::get_evidence(){
	std::ifstream in("out"); 
	char temp[20],buffer[100];
	in.getline(temp,20); 
	in.getline(buffer,100);
	int i=0;
	while(buffer[i]!='0'){
		if(buffer[i]=='-')
			_evidence+="!";
		else if(buffer[i]>='1' && buffer[i]<='9')
			_evidence+="true ";
		i++;
	}
}

void olg_check::generate_evidence(std::vector<edge_set> prefix, std::vector<edge_set> scc)
{
  int size = prefix.size();
  std::string pre="";
  for(int i = 0; i < size; i++)
  {
    std::string s = set_to_string(prefix[i]); //std::cout<<s<<" --s\n";
	if(s!=pre)
    	_evidence += s + ", ";
	pre=s;
  }
  std::string loop = "";
  size = scc.size();
  for(int i = size - 1; i >= 0; i --)
  {
     loop += set_to_string(scc[i]); //std::cout<<loop<<" --loop\n";
     if(i != 0)
       loop += ", ";
  }
  _evidence += "(" + loop + ")";
}

std::string olg_check::set_to_string(edge_set atoms)
{
  std::string res = "";
  edge_set::iterator it;
  for(it = atoms.begin(); it != atoms.end(); it ++)
  {
    res += spot::str_psl(**it);
  }
  return res;
}

spot::formula * olg_check::unique(spot::formula * in){
	for(std::vector<spot::formula *>::iterator it=all_formula.begin();it!=all_formula.end();it++){
		if(*in == **it)
			return *it;
	}
	all_formula.push_back(in);
	return in;
}

bool olg_check::scc_sat(spot::formula *f, edge_set *s){
	switch(f->kind()){
		case spot::op::And:{
			spot::formula *left=new spot::formula(f->operator[](0));
			spot::formula *right=new spot::formula(f->all_but(0)); //std::cout<<"scc_sat1\n";
			return scc_sat(unique(left),s) && scc_sat(unique(right),s);
		}
		case spot::op::Or:{
			spot::formula *left=new spot::formula(f->operator[](0));
			spot::formula *right=new spot::formula(f->all_but(0)); //std::cout<<"scc_sat2\n";
			return scc_sat(unique(left),s) || scc_sat(unique(right),s);
		}
		case spot::op::U:
    	case spot::op::R:{
			spot::formula *sub=new spot::formula(f->operator[](1)); //std::cout<<"scc_sat3\n";
			return scc_sat(unique(sub),s);
		} 
    	case spot::op::X:{
			spot::formula *sub=new spot::formula(f->operator[](0)); //std::cout<<"scc_sat4\n";
			return scc_sat(unique(sub),s);
		}
		case spot::op::tt:
			{ /*std::cout<<"scc_sat5\n";*/ 	return true;}
		case spot::op::ff:
			{ /*std::cout<<"scc_sat6\n";*/ return false;}
		default:
      		{ /*std::cout<<"scc_sat7\n";*/ return s->find (f) != s->end (); }
	}
}

hash_set<spot::formula *> olg_check::and_to_set(spot::formula *in){
	hash_set<spot::formula *> result, result1, result2;

	if(in->kind()!=spot::op::And)
		result.insert(in);
	else{
		spot::formula *l=new spot::formula(in->operator[](0));
		spot::formula *r=new spot::formula(in->all_but(0));
		result1=and_to_set(unique(l));
		result2=and_to_set(unique(r));
		result.insert(result1.begin(), result1.end());
    	result.insert(result2.begin(), result2.end());
		result1.clear();
    	result2.clear();
	}
	return result;
}

spot::formula olg_check::trans_F_G(spot::formula to_trans){
	if(to_trans.kind()==spot::op::ap){
		return to_trans;
	}
	if(to_trans.kind()==spot::op::tt | to_trans.kind()==spot::op::ff){
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
