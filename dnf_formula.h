#ifndef DNF_FORMULA
#define DNF_FORMULA

#include <iostream>
#include <spot/tl/print.hh>
#include <spot/tl/formula.hh>
#include "dnf_clause.h"
#include <vector>
#include <hash_map>
#include <hash_set>
#include "hash_map.h"
#include "hash_set.h"

class dnf_formula{
	public:
		typedef hash_set<dnf_clause, dnf_clause::dnf_clause_hash> dnf_clause_set;
		typedef hash_set<spot::formula *> next_set;
		typedef hash_map<spot::formula *, dnf_formula *> af_dnf_map;
		typedef hash_map<spot::formula *, dnf_clause_set *> dnf_map;
	
		dnf_formula(spot::formula *);
		dnf_formula (const dnf_formula& orig);
		void build();
		void init();
		std::string to_string ()const;
		dnf_clause_set *find_next ();
		dnf_clause_set *get_next ()const;
		void get_next (dnf_clause_set *dc_set)const;
		dnf_formula *unique ();
		dnf_formula *clone ()const;
		void cross (const dnf_formula *dnf1, const dnf_formula *dnf2, dnf_clause_set *s);
		spot::formula trans_F_G(spot::formula);
		spot::formula simplify_And(spot::formula);
		static dnf_formula *get_dnf(spot::formula *);
		dnf_formula& operator = (const dnf_formula& dnf);
	private:
		spot::formula *_id;
		dnf_formula *_left; // 并集左节点
 		dnf_formula *_right; // 并集右节点
		dnf_formula *_unique; // 对应的dnf唯一标识

	private:
  		static af_dnf_map all_dnfs; //记录所有dnf
  		static dnf_map atomic_dnfs; //保存dnf叶子id和对应的dnf_clause集合
  		static spot::formula AF; //初始化spot_formula, 无实质用处
};

#endif
