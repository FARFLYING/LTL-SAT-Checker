#include <spot/tl/formula.hh>
#include "dnf_clause.h"
#include <iostream>
#include <spot/tl/print.hh>

#define HASH_INIT 1315423911

dnf_clause::dnf_clause(spot::formula *current,spot::formula *next){
	this->current = current;
	this->next = next;
	//print_psl(std::cout,*this->current)<<"\n";
	//print_psl(std::cout,*this->next)<<"\n";
	clc_hash ();
}

void dnf_clause::clc_hash ()
{
  _hash = HASH_INIT;
  _hash = (_hash << 5) ^ (_hash >> 27) ^ (size_t) current;
  _hash = (_hash << 5) ^ (_hash >> 27) ^ (size_t) next;
}

bool dnf_clause::operator == (const dnf_clause& dc) const
{
  return current == dc.current && next == dc.next;
}

dnf_clause& dnf_clause::operator = (const dnf_clause& dc)
{
  if (this != &dc)
    {
      this->current = dc.current;
      this->next = dc.next;
      this->_hash = dc._hash;
    }
  return *this;
}

bool dnf_clause::operator< (const dnf_clause& dc) const{
	//spot::formula f1=spot::formula(*next);
	//spot::formula f2=spot::formula(*dc.next);
	return get_length(*next) < get_length(*dc.next);
}

int dnf_clause::get_length(spot::formula in)const{
	switch (in.kind()){
		case spot::op::ap:{
			return 1;	
		}
		case spot::op::Not:{
			return 1+get_length(in.operator[](0));
		}
		case spot::op::G:{
			return 	2+get_length(in.operator[](0));
		}
		case spot::op::F:{
			return 	2+get_length(in.operator[](0));
		}
		case spot::op::X:{
			return 1+get_length(in.operator[](0));
		}
		case spot::op::R:{
			return 1+get_length(in.operator[](0))+get_length(in.operator[](1));
		}
		case spot::op::U:{
			return 1+get_length(in.operator[](0))+get_length(in.operator[](1));
		}
		case spot::op::Or:{
			return 1+get_length(in.operator[](0))+get_length(in.all_but(0));
		}
		case spot::op::And:{
			return 1+get_length(in.operator[](0))+get_length(in.all_but(0));
		}
		default:{
			return 0;		
		}
	}
}

std::string dnf_clause::to_string () const
{
  return "[ " + spot::str_psl(*current) + " -> " + spot::str_psl(*next) + " ]";
}

