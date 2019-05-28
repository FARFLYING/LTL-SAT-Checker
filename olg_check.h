#ifndef OLG_CHECK
#define OLG_CHECK

#include <iostream>
#include <spot/tl/print.hh>
#include <spot/tl/formula.hh>
#include <stack>
#include "olg_formula.h"
#include "dnf_formula.h"

class olg_check{
	public:
		olg_check(spot::formula);
		~olg_check();
		bool check();
		spot::formula clone(spot::formula);
		spot::formula trans_F_G(spot::formula);

		typedef hash_set<spot::formula *> edge_set;
		typedef hash_set<spot::formula *> dnf_set;
  		typedef hash_set<spot::formula *> afp_set;
  		typedef hash_map<spot::formula *, int> timestamp;
  		typedef hash_map<dnf_formula *, edge_set *> scc_edge;
	private:
		spot::formula formula_check;

		static timestamp low; //节点搜索的次序编号(时间戳)
  		static timestamp dfn; //节点或节点的子树能够追溯到的最早的栈中节点的次序号

		int _index; //节点访问次序
  		std::stack<spot::formula *> _stk; //栈
  		dnf_set _instk; // 记是否在栈中

  		std::string _result; //记录sat证据
  		scc_edge _scc; //记录scc边的信息
};

#endif
