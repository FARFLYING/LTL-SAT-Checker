#include <iostream>
#include <string>
#include <spot/tl/formula.hh>		
#include <spot/tl/print.hh>
#include <algorithm>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <string.h>
#include <spot/tl/simplify.hh>
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
	ofp=build_ofp(input);
	cout<<to_ofp_string(ofp)<<"  --ofp\n";
}

olg_formula::~olg_formula(){
	//olgFormula.~formula();
}

spot::formula olg_formula::build_olg_formula(spot::formula toBuild){
	spot::formula olg;
	
	spot::tl_simplifier simp;
	toBuild=simp.negative_normal_form(toBuild);
	//print_psl(std::cout,toBuild)<<"  --olg NF"<<"\n";

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
	ifstream in("out"); //cout<<"1\n";
	in.getline(buffer,100); //cout<<"2\n";
	in.close();     //cout<<"3\n";
	//string result=buffer;
	if(strcmp(buffer,"SAT")==0)
		return true;
	else
		return false;
}

olg_item olg_formula::build_item(spot::formula ap_name,int pos,freqkind freq){
	olg_item item; 
	item.ap=ap_name;
	item.start=pos;
	item._freq=freq;
	return item;
}

olg_formula_position olg_formula::build_ofp(spot::formula toBuild){
	olg_formula_position ofp;
	ofp._left=ofp._right=NULL; 
	switch (toBuild.kind()){
		case spot::op::tt:{
			ofp._op=spot::op::tt;
			break;		
		}
		case spot::op::ff:{
			ofp._op=spot::op::ff;
			break;
		}
		case spot::op::Not:{
			ofp._op=spot::op::Not;
			ofp.atom=build_item(toBuild.operator[](0),0,Once);
			ofp.all_atom.push_back(ofp.atom);
			break;
		}
		case spot::op::ap:{
			ofp._op=spot::op::ap;  
			ofp.atom=build_item(toBuild,0,Once);
			ofp.all_atom.push_back(ofp.atom);  //cout<<atom_to_string(ofp.atom)<<"\n";  
			break;
		}
		case spot::op::And:{
			ofp._op=spot::op::And;
			ofp._left=new olg_formula_position(build_ofp(toBuild.operator[](0)));
			ofp._right=new olg_formula_position(build_ofp(toBuild.all_but(0)));
			for(int i=0;i<ofp._left->all_atom.size();i++){
				{ofp.all_atom.push_back(ofp._left->all_atom[i]); /*cout<<atom_to_string(ofp._left->all_atom[i])<<"  And\n";*/}
			}
			for(int i=0;i<ofp._right->all_atom.size();i++){
				{ofp.all_atom.push_back(ofp._right->all_atom[i]); /*cout<<atom_to_string(ofp._right->all_atom[i])<<"  And\n";*/}
			}
			break;
		}
		case spot::op::Or:{
			ofp._op=spot::op::Or;
			bool flag=true;
			olg_formula_position ofp1=build_ofp(toBuild.operator[](0));
			olg_formula_position ofp2=build_ofp(toBuild.all_but(0));
			std::vector<olg_item> temp_atoms;
			for(int i=0;i<ofp1.all_atom.size();i++)
				{temp_atoms.push_back(ofp1.all_atom[i]); /*cout<<atom_to_string(ofp1.all_atom[i])<<" Or\n";*/}
			for(int i=0;i<ofp2.all_atom.size();i++)
				{temp_atoms.push_back(ofp2.all_atom[i]); /*cout<<atom_to_string(ofp2.all_atom[i])<<" Or\n";*/}
			for(int i=0;i<temp_atoms.size();i++){
				if(temp_atoms[i].start!=temp_atoms[0].start){
					flag=false; //cout<<"0\n";
					break;
				}
			}
			if(flag==true){
				ofp._left=new olg_formula_position(ofp1);
				ofp._right=new olg_formula_position(ofp2); //cout<<"1\n";
			}
			else{
				pos_Or(ofp1);
				pos_Or(ofp2);
				ofp._left=new olg_formula_position(ofp1);
				ofp._right=new olg_formula_position(ofp2);
			}
			for(int i=0;i<ofp._left->all_atom.size();i++){
				ofp.all_atom.push_back(ofp._left->all_atom[i]);
			}
			for(int i=0;i<ofp._right->all_atom.size();i++){
				ofp.all_atom.push_back(ofp._right->all_atom[i]);
			}
			break;
		}
		case spot::op::X:{
			ofp=build_ofp(toBuild.operator[](0));
			pos_X(ofp);
			break;
		}
		case spot::op::U:{
			ofp=build_ofp(toBuild.operator[](1));
			pos_U(ofp);   //cout<<to_ofp_string(ofp)<<"  --U\n";
			break;
		}
		case spot::op::R:{	
			ofp=build_ofp(toBuild.operator[](1));
			if(toBuild.operator[](0).kind()==spot::op::ff){
				pos_R(ofp);
			}
			break;
		}
	}
	return ofp;
}

void olg_formula::pos_X(olg_formula_position &f){
	if(f.atom.start>=0) f.atom.start+=1;
	if(f._left!=NULL) pos_X(*f._left);
	if(f._right!=NULL) pos_X(*f._right);
	for(int i=0;i<f.all_atom.size();i++)
		if(f.all_atom[i].start>=0) f.all_atom[i].start+=1;
}

void olg_formula::pos_U(olg_formula_position &f){
	if(f.atom.start>=0) f.atom.start=-1;
	if(f._left!=NULL) pos_U(*f._left);
	if(f._right!=NULL) pos_U(*f._right);
	for(int i=0;i<f.all_atom.size();i++)
		if(f.all_atom[i].start>=0) f.all_atom[i].start=-1;;
}

void olg_formula::pos_R(olg_formula_position &f){
	if(f.atom._freq==Once){
		if(f.atom.start>=0)
			f.atom._freq=More;
		else
			f.atom._freq=Inf;
	}
	if(f._left!=NULL) pos_R(*f._left);
	if(f._right!=NULL) pos_R(*f._right);
	for(int i=0;i<f.all_atom.size();i++){
		if(f.all_atom[i]._freq==Once){
		if(f.all_atom[i].start>=0)
			f.all_atom[i]._freq=More;
		else
			f.all_atom[i]._freq=Inf;
	}
	}
}

void olg_formula::pos_Or(olg_formula_position &f){
	f.atom.start=-1;
	f.atom._freq=Once;
	if(f._left!=NULL) pos_Or(*f._left);
	if(f._right!=NULL) pos_Or(*f._right);
	for(int i=0;i<f.all_atom.size();i++){
		f.all_atom[i].start=-1;
		f.all_atom[i]._freq=Once;
	}
}

std::string olg_formula::to_ofp_string(olg_formula_position in){
	std::string result="";
	switch (in._op){
		case spot::op::tt:{
			result="True";
			break;
		}
		case spot::op::ff:{
			result="False";
			break;
		}
		case spot::op::ap:{
			result+=atom_to_string(in.atom);
			break;
		}
		case spot::op::Not:{
			result+="!"+atom_to_string(in.atom);
			break;
		}
		case spot::op::And:{
			result+="("+to_ofp_string(*in._left)+" & "+to_ofp_string(*in._right)+")";
			break;
		}
		case spot::op::Or:{
			result+="("+to_ofp_string(*in._left)+" | "+to_ofp_string(*in._right)+")";
			break;
		}
	}
	return result;
}

std::string olg_formula::atom_to_string(olg_item a){
	std::string result="";
	result+="< ";
	result+=spot::str_psl(a.ap);

	if(a.start<0) result+=", ⊥";
	else result+=", "+std::to_string(a.start);
	
	if (a._freq == Once) result += ", −";
	else if (a._freq == More) result += ", ≥";
	else result += ", inf";

	result += " >";
	
	return result;
}

bool olg_formula::unsat(){
	//print_psl(std::cout,projection_i(ofp,0))<<"  --projection_i\n";
	for(int i=0;i<ofp.all_atom.size();i++){
		if(ofp.all_atom[i].start>=0){
			if(!is_proj_sat(projection_i(ofp,ofp.all_atom[i].start)))
				return true;		
		}	
	}
	
	std::vector<olg_item> S;
	for(int i=0;i<ofp.all_atom.size();i++)
		if(ofp.all_atom[i]._freq==More)
			S.push_back(ofp.all_atom[i]); cout<<S.size()<<"  --S.size\n";
	
	if(S.size()==0)
		return false;

	if(!is_proj_sat(projection_s(ofp,S)))
		return true;
	
	std::vector<olg_item> S1;
	for(int i=0;i<S.size();i++)
		if(S[i].start==0)
			S1.push_back(S[i]);  cout<<S1.size()<<"  S1.size\n";
	if(S1.size()>0){
		for(int i=0;i<ofp.all_atom.size();i++){
			if(ofp.all_atom[i].start<0){
				std::vector<olg_item> temp(S1);
				temp.push_back(ofp.all_atom[i]);
				if(!is_proj_sat(projection_s(ofp,temp)))
					return true;
			}
		}
	}


	for(int i=0;i<ofp.all_atom.size();i++){
		if(ofp.all_atom[i]._freq==Inf){
			std::vector<olg_item> temp(S);
			temp.push_back(ofp.all_atom[i]);
			if(!is_proj_sat(projection_s(ofp,temp)))
				return true;
		}
	}
	
	return false;
}

bool olg_formula::is_belong_S(olg_item l,std::vector<olg_item> S){
	for(int i=0;i<S.size();i++){
		if(l.ap==S[i].ap & l.start==S[i].start & l._freq==S[i]._freq)
			return true;
	}
	return false;
}

spot::formula olg_formula::projection_s(olg_formula_position ofp,std::vector<olg_item> S){
	spot::formula result;
	switch(ofp._op){
		case spot::op::ap:{
			if(is_belong_S(ofp.atom,S))
				result=ofp.atom.ap;
			else
				result=spot::formula::tt();
			break;
		}
		case spot::op::Not:{
			if(is_belong_S(ofp.atom,S))
				result=spot::formula::Not(ofp.atom.ap);
			else
				result=spot::formula::tt();
			break;
		}
		case spot::op::And:{
			std::vector<spot::formula> temp;
			temp.push_back(projection_s(*ofp._left,S));
			temp.push_back(projection_s(*ofp._right,S));	
			result=spot::formula::And(temp);
			break;	
		}
		case spot::op::Or:{
			std::vector<spot::formula> temp;
			temp.push_back(projection_s(*ofp._left,S));
			temp.push_back(projection_s(*ofp._right,S));	
			result=spot::formula::Or(temp);
			break;	
		}
	}
	print_psl(std::cout,result)<<"  --projection_s\n";
	return result;
}

bool olg_formula::is_proj_sat(spot::formula input){
	write_dimacs(convert_to_CNF(input));
	system("minisat cnf.dimacs out");
	char buffer[100]; 
	ifstream in("out");
	in.getline(buffer,100); 
	in.close();     
	if(strcmp(buffer,"SAT")==0)
		return true;
	else
		return false;
}

spot::formula olg_formula::projection_i(olg_formula_position ofp,int i){
	spot::formula result;
	switch(ofp._op){
		case spot::op::tt:{
			//TODO
		}
		case spot::op::ap:{
			if((ofp.atom.start==i)|(ofp.atom.start<i && ofp.atom._freq==More))
				result=ofp.atom.ap;
			else
				result=spot::formula::tt();
			break;
		}
		case spot::op::Not:{
			if((ofp.atom.start==i)|(ofp.atom.start<i && ofp.atom._freq==More))
				result=spot::formula::Not(ofp.atom.ap);
			else
				result=spot::formula::tt();
			break;
		}
		case spot::op::And:{
			std::vector<spot::formula> temp;
			temp.push_back(projection_i(*ofp._left,i));
			temp.push_back(projection_i(*ofp._right,i));	
			result=spot::formula::And(temp);
			break;	
		}
		case spot::op::Or:{
			std::vector<spot::formula> temp;
			temp.push_back(projection_i(*ofp._left,i));
			temp.push_back(projection_i(*ofp._right,i));	
			result=spot::formula::Or(temp);
			break;	
		}
	}
	print_psl(std::cout,result)<<"  --projection_i\n";
	return result;
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



