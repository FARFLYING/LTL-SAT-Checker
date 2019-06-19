#ifndef OLG_FORMULA
#define OLG_FORMULA

#include <iostream>
#include <spot/tl/print.hh>
#include <spot/tl/formula.hh>

enum freqkind{
	Once,More,Inf		
};

typedef struct{
		freqkind _freq;
		int start;
		spot::formula ap;
}olg_item;

typedef struct olg_formula_position{
		std::vector<olg_item> all_atom;
		olg_item atom;
		spot::op _op;
		olg_formula_position *_left;
		olg_formula_position *_right;
}olg_formula_position;

class olg_formula{
	private:
		//spot::formula _left;
		//spot::formula _right;
		spot::formula olgFormula;
		olg_formula_position ofp;
	public:
		spot::op top_most;
		olg_formula(spot::formula);
		//olg_formula();
		~olg_formula();
		spot::formula build_olg_formula(spot::formula);
		std::vector<std::string> split_formula(spot::formula);
		spot::formula convert_to_CNF(spot::formula);
		void get_all_apName(spot::formula);
		void write_dimacs(spot::formula);
		bool sat();
		bool unsat();
		olg_formula_position build_ofp(spot::formula);
		olg_item build_item(spot::formula ap_name,int pos,freqkind freq);
		std::string to_ofp_string(olg_formula_position);
		std::string atom_to_string(olg_item);
		void pos_X(olg_formula_position &);
		void pos_U(olg_formula_position &);
		void pos_R(olg_formula_position &);
		void pos_Or(olg_formula_position &);
		spot::formula projection_i(olg_formula_position,int);
		bool is_proj_sat(spot::formula);
		spot::formula projection_s(olg_formula_position,std::vector<olg_item>);
		bool is_belong_S(olg_item,std::vector<olg_item>);
};

#endif
