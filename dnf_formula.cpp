#include "dnf_formula.h"
#include <iostream>
#include <spot/tl/formula.hh>
#include <spot/tl/print.hh>
#include <spot/tl/simplify.hh>
using namespace std;

dnf_formula::af_dnf_map dnf_formula::all_dnfs;
dnf_formula::dnf_map dnf_formula::atomic_dnfs;
spot::formula dnf_formula::AF;

void dnf_formula::init ()
{
  _left = _right = _unique = NULL;
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
	spot::tl_simplifier simp;
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
		case spot::op::U:{// DNF(φ1 U φ2) = DNF(φ1 ∧ X(φ1 U φ2)) ∪ DNF(φ2)
			vector<spot::formula> temp;
			spot::formula *afp=new spot::formula(simp.simplify(spot::formula::X(*_id))); //print_psl(std::cout,*afp)<<"\n";
			temp.push_back(*afp);
			temp.push_back(_id->operator[](0));
			afp=new spot::formula(simp.simplify(spot::formula::And(temp)));   //print_psl(std::cout,*afp)<<"\n";
			_left=dnf_formula (*afp).unique();
			_right=dnf_formula(_id->operator[](1)).unique();
			break;
		}
		case spot::op::R:{// DNF(φ1 R φ2) = DNF(φ1 ∧ φ2) ∪ DNF(φ2 ∧ X(φ1 R φ2))
			vector<spot::formula> temp;
			temp.push_back(_id->operator[](0));
			temp.push_back(_id->operator[](1));
			spot::formula *l_afp=new spot::formula(simp.simplify(spot::formula::And(temp)));
			spot::formula *r_afp=new spot::formula(simp.simplify(spot::formula::X(*_id)));
			temp.clear();
			temp.push_back(_id->operator[](1));
			temp.push_back(*r_afp);
			r_afp=new spot::formula(simp.simplify(spot::formula::And(temp)));
			_left=dnf_formula(*l_afp).unique();
			_right=dnf_formula(*r_afp).unique();
			break;
		}
		case spot::op::Or:{// DNF(φ1 ∨ φ2) = DNF(φ1) ∪ DNF(φ2)
			_left=dnf_formula(_id->operator[](0)).unique();
			_right=dnf_formula(_id->all_but(0)).unique();
			break;
		}
		case spot::op::And:{// DNF(φ1∧φ2) = {(α1∧α2)∧X(ψ1∧ψ2) | ∀i = 1,2.αi∧X(ψi) ∈ DNF(φi)}
			dnf_clause_set *dc_set = this->find_next ();
			if (dc_set->empty ()){
				dnf_formula *l_dnf = dnf_formula(_id->operator[](0)).unique ();
				dnf_formula *r_dnf = dnf_formula(_id->all_but(0)).unique ();
				cross (l_dnf, r_dnf, dc_set);
			}
			break;
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

void dnf_formula::cross (const dnf_formula *dnf1, const dnf_formula *dnf2, dnf_clause_set *s)
{
  if (dnf1 == NULL || dnf2 == NULL)
    {
      cout<<"formula in cross is NULL!\n";
      return;
    }
  if (dnf1->_left != NULL)
    {
      cross (dnf1->_left, dnf2, s);
      cross (dnf1->_right, dnf2, s);
    }
  else if (dnf2->_left != NULL)
    {
      cross (dnf1, dnf2->_left, s);
      cross (dnf1, dnf2->_right, s);
    }
  else
    {
	  spot::tl_simplifier simp;
      dnf_clause_set *s1 = atomic_dnfs[dnf1->_id];
      dnf_clause_set *s2 = atomic_dnfs[dnf2->_id];
      dnf_clause_set::const_iterator it1, it2;
      for (it1 = s1->begin (); it1 != s1->end (); it1++)
        for (it2 = s2->begin (); it2 != s2->end (); it2++)
          {
            //aalta_formula *caf = aalta_formula::simplify_and_weak (it1->current, it2->current);
			vector<spot::formula> temp;
			temp.push_back(*it1->current);
			temp.push_back(*it2->current);
			spot::formula *caf=new spot::formula(simp.simplify(spot::formula::And(temp)));
            if (caf->kind()==spot::op::ff)
            	continue;
            //aalta_formula *naf = aalta_formula::simplify_and_weak (it1->next, it2->next);
			temp.clear();
			temp.push_back(*it1->next);
			temp.push_back(*it2->next);
			spot::formula *naf=new spot::formula(simp.simplify(spot::formula::And(temp)));
            s->insert (dnf_clause (caf, naf));
          }
    }
}

dnf_formula* dnf_formula::unique ()
{
  if (_unique == NULL)
    {
      af_dnf_map::const_iterator it = all_dnfs.find (_id);
      if (it != all_dnfs.end ())
        _unique = it->second;
      else
        {
          _unique = this->clone ();
          all_dnfs[_id] = _unique;
        }
    }
  return _unique;
}

dnf_formula * dnf_formula::clone () const
{
  return new dnf_formula (*this);
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
