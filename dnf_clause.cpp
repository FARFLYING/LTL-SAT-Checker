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
}

void dnf_clause::clc_hash ()
{
  _hash = HASH_INIT;
  _hash = (_hash << 5) ^ (_hash >> 27) ^ (size_t) current;
  _hash = (_hash << 5) ^ (_hash >> 27) ^ (size_t) next;
}

bool dnf_clause::operator == (const dnf_clause& dc) const
{
  return *current == *dc.current && *next == *dc.next;
}

std::string dnf_clause::to_string () const
{
  return "[ " + spot::str_psl(*current) + " -> " + spot::str_psl(*next) + " ]";
}

