#include <iostream>
#include <string>
#include <spot/tl/formula.hh>		
#include <spot/tl/print.hh>
#include <algorithm>
#include <fstream>
#include <vector>
#include <cstdlib>
#include "olg_formula.h"
using namespace std;

/*olg_formula::olg_formula(){
	olg=NULL;
}*/

olg_formula::olg_formula(spot::formula input){
	top_most=input.kind();

	//cout<<"top_most:"<<input.kindstr().c_str()<<endl;
	
	olgFormula=build_olg_formula(input);
	//print_psl(std::cout,input)<<"\n";
	print_psl(std::cout,olgFormula)<<"  --olg\n";
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

}

spot::formula olg_formula::convert_to_CNF(spot::formula toConvert){
	if(toConvert.kind()==spot::op::ap)
		return toConvert;
	else if(toConvert.kind()==spot::op::And){
		vector<spot::formula> sub;
		for(int i=0;i<toConvert.size();i++){
			sub.push_back(convert_to_CNF(toConvert.operator[](i)));
		}
		return spot::formula::And(sub);
	}
	else if(toConvert.kind()==spot::op::Or){
		bool is_all_literal=true;
		for(int i=0;i<toConvert.size();i++){
			if(!toConvert.operator[](i).is_literal()){
				is_all_literal=false;
				break;			
			}
		}
		if(is_all_literal)
			return toConvert;

		spot::formula p=convert_to_CNF(toConvert.operator[](0)); //print_psl(std::cout,p)<<"  --convert_p\n";
		spot::formula q=convert_to_CNF(toConvert.all_but(0));   //print_psl(std::cout,q)<<"  --convert_q\n";
		vector<spot::formula> sub,temp;
		for(int i=0;i<p.size();i++){
			for(int j=0;j<q.size();j++){
				temp.clear();
				//print_psl(std::cout,p.operator[](i))<<"  --p_child_"<<i<<"\n";
				temp.push_back(p.operator[](i)); 
				//print_psl(std::cout,q)<<"  --q_child_"<<j<<"\n";  
				temp.push_back(q.operator[](j));   
				sub.push_back(spot::formula::Or(temp)); 
				//print_psl(std::cout,spot::formula::Or(temp))<<"  --or_temp"<<"\n";  
			}
		}
		return spot::formula::And(sub);
	}
	else if(toConvert.kind()==spot::op::Not){
		if(toConvert.is_literal())
			return toConvert;
		else{
			spot::formula child_of_Not=toConvert.get_child_of(spot::op::Not);
			if(child_of_Not.kind()==spot::op::Not)
				return convert_to_CNF(child_of_Not.get_child_of(spot::op::Not));
			else if(child_of_Not.kind()==spot::op::And){
				spot::formula p=spot::formula::Not(child_of_Not.operator[](0));
				spot::formula q=spot::formula::Not(child_of_Not.all_but(0));
				vector<spot::formula> temp;
				temp.push_back(p);
				temp.push_back(q);
				return convert_to_CNF(spot::formula::Or(temp));
			}
			else if(child_of_Not.kind()==spot::op::Or){
				spot::formula p=spot::formula::Not(child_of_Not.operator[](0));
				spot::formula q=spot::formula::Not(child_of_Not.all_but(0));
				vector<spot::formula> temp;
				temp.push_back(p);
				temp.push_back(q);
				return convert_to_CNF(spot::formula::And(temp));			
			}
		}
	}
	
}

static vector<string> all_apName;
void olg_formula::get_all_apName(spot::formula toGet){
	if(toGet.kind()==spot::op::ap){
		if(find(all_apName.begin(),all_apName.end(),toGet.ap_name())==all_apName.end()){
			all_apName.push_back(toGet.ap_name());
		}	
		return;
	}
	for(int i=0;i<toGet.size();i++){
		get_all_apName(toGet.operator[](i));
	}
}

int flagNum(string ap[],string t,int n){
	int i;
	for(i=0;i<n;i++){
		if(ap[i]==t)
			break;
	}
	return i+1;
}

void olg_formula::write_dimacs(spot::formula toWrite){
	int valNum,clNum;

	if(toWrite.kind()==spot::op::ap | toWrite.kind()==spot::op::Not | toWrite.kind()==spot::op::Or)
		clNum=1;
	else{
		clNum=toWrite.size();
	}

	get_all_apName(toWrite);
	valNum=all_apName.size();

	string ap[valNum];
	vector<string>::iterator iter;
	int i;
	for (iter=all_apName.begin(),i=0;iter!=all_apName.end();iter++,i++)
	{
		ap[i]=*iter;
	}

	ofstream in;
	in.open("cnf.dimacs",ios::trunc);

	in<<"p cnf "<<valNum<<" "<<clNum<<"\n";
	

	if(toWrite.kind()==spot::op::ap){
		int flag=flagNum(ap,toWrite.ap_name(),valNum);
		in<<flag<<" "<<"0"<<"\n";
	}
	else if(toWrite.kind()==spot::op::Not){
		int flag=flagNum(ap,toWrite.get_child_of(spot::op::Not).ap_name(),valNum);
		in<<"-"<<flag<<" "<<"0"<<"\n";		
	}
	else if(toWrite.kind()==spot::op::Or){
		for(i=0;i<toWrite.size();i++){
			if(toWrite.operator[](i).kind()==spot::op::ap){
				int flag=flagNum(ap,toWrite.operator[](i).ap_name(),valNum);
				in<<flag<<" ";			
			}
			else if(toWrite.operator[](i).kind()==spot::op::Not){
				int flag=flagNum(ap,toWrite.operator[](i).get_child_of(spot::op::Not).ap_name(),valNum);
				in<<"-"<<flag<<" ";			
			}		
		}
		in<<"0"<<"\n";
	}
	else{
		for(i=0;i<toWrite.size();i++){
			spot::formula temp=toWrite.operator[](i);
			if(temp.kind()==spot::op::ap){
					int flag=flagNum(ap,temp.ap_name(),valNum);
					in<<flag<<" "<<"0"<<"\n";
			}
			else if(temp.kind()==spot::op::Not){
				int flag=flagNum(ap,temp.get_child_of(spot::op::Not).ap_name(),valNum);
				in<<"-"<<flag<<" "<<"0"<<"\n";	
			}
			else{
			for(int j=0;j<temp.size();j++){
				if(temp.operator[](j).kind()==spot::op::ap){
					int flag=flagNum(ap,temp.operator[](j).ap_name(),valNum);
					in<<flag<<" ";			
				}
				else if(temp.operator[](j).kind()==spot::op::Not){
					int flag=flagNum(ap,temp.operator[](j).get_child_of(spot::op::Not).ap_name(),valNum);
					in<<"-"<<flag<<" ";			
				}		
			}
			in<<"0"<<"\n";
			}
		}
	}
	
	in.close();

}

bool olg_formula::sat(){
	if(top_most==spot::op::tt)
		return true;
	if(top_most==spot::op::ff)
		return false;
	//Minisat::Solver S;
	spot::formula CNFformat=convert_to_CNF(olgFormula);
	print_psl(std::cout,CNFformat)<<"  --CNF\n";
	write_dimacs(CNFformat);
	system("minisat cnf.dimacs out");
	char buffer[100];
	ifstream in("out");
	in.getline(buffer,100);
	string result=buffer;
	if(result=="SAT")
		return true;
	return false;
}

/*
bool olg_formula::unsat(){
	if(top_most==spot::op::ff)
		return true;
	else if(top_most==spot::op::tt)
		return false;
}*/

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



