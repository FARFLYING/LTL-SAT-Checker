#include "dnf_formula.h"
#include <iostream>
#include <spot/tl/formula.hh>
#include <spot/tl/print.hh>
using namespace std;

dnf_formula::af_dnf_map dnf_formula::all_dnfs;
dnf_formula::dnf_map dnf_formula::atomic_dnfs;
spot::formula dnf_formula::AF;

void dnf_formula::init ()
{
  _left = _right = NULL;
  _id = NULL;
}

dnf_formula::dnf_formula(spot::formula input){
	this->init ();
	_id=new spot::formula(input);
	//print_psl(std::cout,*_id)<<"\n";
	build();
}

void dnf_formula::build(){
	if (_id == NULL) return;
	af_dnf_map::const_iterator it = all_dnfs.find (_id);
	if (it != all_dnfs.end ()){
		*this = *(it->second);
		return;
	}
	switch(_id->kind()){
		case spot::op::X:{
			dnf_clause_set *dc_set = this->find_next ();
			if (dc_set->empty ()){
				spot::formula *cur=new spot::formula(spot::formula::tt());
				spot::formula *nxt=new spot::formula(_id->operator[](0));
				dc_set->insert (dnf_clause (cur,nxt));
			}	
        	break;
		}
		case spot::op::U:{
			
		}
		default:
      	{ // DNF(α) = {α ∧ X(True)}
        	dnf_clause_set *dc_set = this->find_next ();
        	if (dc_set->empty () && _id->kind()!= spot::op::ff){
				spot::formula *cur=new spot::formula(*_id);
				spot::formula *nxt=new spot::formula(spot::formula::tt());
				dc_set->insert (dnf_clause (cur,nxt));
			}
        	break;
      	}
	}
}

dnf_formula::dnf_clause_set* dnf_formula::find_next ()
{
	dnf_map::const_iterator it = atomic_dnfs.find (_id);
	if (it == atomic_dnfs.end ())
    {
		dnf_clause_set *tmp = new dnf_clause_set ();
      	atomic_dnfs[_id] = tmp;
      	_left = _right = NULL;
      	return tmp;
    }
  	return it->second;
}

dnf_formula::dnf_clause_set * dnf_formula::get_next () const
{
	dnf_map::iterator it = atomic_dnfs.find (_id);
  	if (it == atomic_dnfs.end ())
    {
		atomic_dnfs[_id] = new dnf_clause_set ();
      	it = atomic_dnfs.find (_id);
    }
  	if (it->second->empty ())
    {
      	if (_left != NULL)
       		_left->get_next (it->second);
      	if (_right != NULL)
        	_right->get_next (it->second);
    }
  	return it->second;
}

void dnf_formula::get_next (dnf_clause_set *dc_set) const
{
  dnf_map::iterator it = atomic_dnfs.find (_id);
  if (it == atomic_dnfs.end ())
    {
      _left->get_next (dc_set);
      _right->get_next (dc_set);
    }
  else
    {
      for (dnf_clause_set::const_iterator iter = it->second->begin (); iter != it->second->end (); iter++)
        dc_set->insert (*iter);
    }
}

std::string dnf_formula::to_string () const{
	dnf_clause_set *dc_set = this->get_next ();
	std::string ret;
	//cout<<"dnf_formula:  "<<dc_set->size ()<<"\n";
	if (dc_set->size () > 0){
		ret += "\n{";
      	for (dnf_clause_set::const_iterator it = dc_set->begin (); it != dc_set->end (); it++)
        	ret += "\n\t" + it->to_string ();
      	ret += "\n}";
	}
	return ret;
}
