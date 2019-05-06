#include <iostream>
#include <string>
#include <spot/tl/formula.hh>		
#include <spot/tl/print.hh>
#include "olg_formula.h"
using namespace std;

/*olg_formula::olg_formula(){
	olg=NULL;
}*/

olg_formula::olg_formula(spot::formula input){
	top_most=input.kind();

	//cout<<"top_most:"<<input.kindstr().c_str()<<endl;
	
	olgFormula=build_olg_formula(input);
	print_psl(std::cout,input)<<"\n";
	print_psl(std::cout,olgFormula)<<"\n";
}

olg_formula::~olg_formula(){
	olgFormula.~formula();
}

spot::formula olg_formula::build_olg_formula(spot::formula toBuild){
	//spot::formula _true=spot::parse_formula("true");
	//spot::formula _false=spot::parse_formula("false");
	spot::formula olg;
	
	if(toBuild.is_tt()){
		olg=NULL;
	}
	else if(toBuild.is_ff()){
		olg=spot::formula::ff();
	}
	else if(toBuild.is_literal()){
		olg=toBuild;
	}
	else if(toBuild.kind()==spot::op::X | toBuild.kind()==spot::op::F | toBuild.kind()==spot::op::G){
		olg=build_olg_formula(toBuild.operator[](0));
	}
	else if(toBuild.kind()==spot::op::Or){
		vector<spot::formula> sub_formula_set;
		for(int i=0;i<toBuild.size();i++){
			sub_formula_set.push_back(build_olg_formula(toBuild.operator[](i)));		
		}
		olg=spot::formula::Or(sub_formula_set);
	}
	else if(toBuild.kind()==spot::op::And){
		vector<spot::formula> sub_formula_set;
		for(int i=0;i<toBuild.size();i++){
			sub_formula_set.push_back(build_olg_formula(toBuild.operator[](i)));		
		}
		olg=spot::formula::And(sub_formula_set);
	}
	else if(toBuild.kind()==spot::op::R | toBuild.kind()==spot::op::U){
		olg=build_olg_formula(toBuild.operator[](1));
	}
	return olg;


	//vector<string> str_subformula=split_formula(toBuild);
	/*cout<<str_subformula.size()<<"\n";
	for (vector<string>::iterator iter = str_subformula.begin(); iter < str_subformula.end(); iter++)
    	{
        	//输出*iter才是输出那些字符串
       		cout <<"("<< *iter<<")" << endl;
    	}
	*/
	
}

bool olg_formula::unsat(){
	if(top_most==spot::op::ff)
		return true;
	else if(top_most==spot::op::tt)
		return false;
}

vector<std::string> olg_formula::split_formula(spot::formula toSplit){
	vector<std::string> result;
	std::string s=spot::str_psl(toSplit);
	string flag;
	bool binary=false;

	
	cout<<"print split formula:";
	spot::print_psl(std::cout,toSplit)<<"\n";

	switch (top_most){
		case spot::op::And:
		{
			flag="&";
			break;
		}
		case spot::op::Or:
		{
			flag="|";
			break;
		}
		case spot::op::Implies:
		{
			flag="->";
			binary=true;
			break; 
		}
		case spot::op::Equiv:
		{
			flag="<->";
			binary=true;
			break; 
		}
		case spot::op::Not:
		{
			flag="!";
			break; 
		}
		case spot::op::Xor:
		{
			flag="xor";
			break; 
		}
		case spot::op::U:
		{
			flag="U"; 
			binary=true;
			break;
		}
		case spot::op::W:
		{
			flag="W"; 
			binary=true;
			break;
		}
		case spot::op::M:
		{
			flag="M"; 
			binary=true;
			break;
		}
		case spot::op::R:
		{
			flag="R"; 
			binary=true;
			break;
		}
		case spot::op::X:
		{
			flag="X"; 
			break;
		}
		case spot::op::F:
		{
			flag="F"; 
			break;
		}
		case spot::op::G:
		{
			flag="G"; 	
			break;
		}
		default:
			flag="";
	}

	std::string::size_type pos1, pos2;
    size_t len = s.length();
    pos2 = s.find(flag);
    pos1 = 0;
    while(std::string::npos != pos2)
    {
        result.emplace_back(s.substr(pos1, pos2-pos1));
 
        pos1 = pos2 + flag.size();
        pos2 = s.find(flag, pos1);
    }
    if(pos1 != len)
        result.emplace_back(s.substr(pos1));
	
	return result;
}



