#ifndef DNF_CLAUSE
#define	DNF_CLAUSE

#include <spot/tl/formula.hh>

class dnf_clause{
	public:
		spot::formula *current;
		spot::formula *next;
		size_t _hash; 
		
		dnf_clause(spot::formula *current=NULL,spot::formula *next=NULL);
		void clc_hash ();
		std::string to_string ()const;

		dnf_clause& operator = (const dnf_clause& dc);
		bool operator == (const dnf_clause& dc)const;
		bool operator< (const dnf_clause& dc) const;
		int get_length(spot::formula)const;

		struct dnf_clause_hash
		{

    		size_t operator () (const dnf_clause& clause) const
    		{
      			return clause._hash;
    		}
  		};


};
#endif
