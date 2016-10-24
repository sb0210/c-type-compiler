#include "symtable.h"


	BaseType::BaseType(){}
	BaseType::BaseType(string name){
		this->name = name;
		this->type = 0;
		pab = "base";
	}
	void BaseType::print(){
		cout<<name;
	}
	bool BaseType::equal(Type* type2,int rec){
		if(type2->pab=="base"){
			if(rec==0){
				if((this->name=="int" || this->name=="float") && (type2->name=="int" || type2->name=="float")){
					return true;
				}
			}
			return (this->name==type2->name);
		}
		return false;
	}

	int BaseType::size(){
		if(this->equal(Type_int)){return 4;}
		else if(this->equal(Type_void)){return 0;}
		else {
			return gst->symlist.find(this->name)->second->size;
		}
	}

	Type* BaseType::Dereference(){
		return NULL;
	}
	
	 Type* PointerType::Dereference(){
		if(this->type->pab=="base" && this->type->name=="void"){
			return NULL;
		}
	 	return this->type;
	 }

	Type* ArrayType::Dereference(){
		return this->type;
	}
	PointerType::PointerType(){}
	
	PointerType::PointerType(Type* type){
		this->type = type;
		pab = "pointer";

	}
	void PointerType::print(){
		cout<<"*";
		this->type->print();
	}
	bool PointerType::equal(Type* type2,int rec){
		if(type2->pab=="base"){
			return false;
		}
		if(type2->pab=="pointer"){
			return this->type->equal(type2->type,rec+1);
		}
		if(type2->pab=="array"){
			if(rec==0){
					 return this->type->equal(type2->type,1);
				}
			else{
				return false;
			}	
			// return this->type->equal(type->type2)
		}
		return false;
	}

	int PointerType::size(){
		return 4;
	}

	ArrayType::ArrayType(){}
	ArrayType::ArrayType(int dimension, Type* type){
		this->dimension = dimension;
		this->type = type;
		pab = "array";
	}
	void ArrayType::print(){
		cout<<"array("<<dimension<<", ";
		type->print();
		cout<<")";	
	}
	bool ArrayType::equal(Type* type2, int rec){
		if(type2->pab=="base"){
			return false;
		}
		if(type2->pab=="pointer"){
			return false;
		}
		if(type2->pab=="array"){
			return (this->dimension==type2->dimension) && this->type->equal(type2->type,rec+1);
		}
		return false;
	}

	int ArrayType::size(){
		return this->dimension * this->type->size();
	}

	// Type* getType(Type* type1, Type* type2){
	// 	if(type1->0 && type2->0){
	// 		if(type1->name=="int" && type2->name=="int"){
	// 			return new Type*("int");
	// 		}
	// 		if(type1->name=="float" || type2->name=="float"){
	// 			return new Type*("float");
	// 		}
	// 		return new Type(type1->string);
	// 	}
	// 	if 


	// }
	Record_global::Record_global(){}
	Record_global::Record_global(
		string symbol_name,
		sf_enum sf,
		Type* type,
		int size,
		SymTable_local* symtable
	){
		this->symbol_name=symbol_name;
		this->sf = sf;
		this->type=type;
		this->size=size;
		this->symtable=symtable;
	}
	void Record_global::print(){
		cout<<id<<"\t"<<symbol_name<<"\t"<<sf<<"\t";cout<<size<<"\t";type->print();cout<<endl;
		cout<<endl;
		cout<<"Symbol table for "<<symbol_name<<endl;
		symtable->print();
		cout<<endl<<endl;
	}

	Record_local::Record_local(){}
	Record_local::Record_local(
		string symbol_name,
		// pv_enum pv,
		bool is_param,
		Type* type,
		int size,
		int offset
	){
		this->symbol_name=symbol_name;
		this->is_param = is_param;
		// this->pv = pv;
		this->type=type;
		this->size=size;
		this->offset=offset;
	}
	void Record_local::print(){
		cout<<id<<"\t"<<symbol_name<<"\t"<<is_param<<"\t";cout<<size<<"\t"<<offset<<"\t";type->print();cout<<endl;
	}


	vector<Record_local*> Record_global::params(){
		return this->symtable->params;
	}
	SymTable_global::SymTable_global(){
		id=0;
	}
	void SymTable_global::add(string record_name,Record_global* record){
		record->id=id;
		id++;
		this->symlist.insert(std::pair<string,Record_global*>(record_name,record));
	}
	void SymTable_global::print(){
   		std::map<string,Record_global*>::iterator it = symlist.begin();
   		cout<<"Printing Global Table"<<endl;
   		cout<<"id\tname\tSF\tsize\ttype"<<endl;
   		vector<Record_global*> rc;
		for (it=symlist.begin(); it!=symlist.end(); ++it){
    		// it->second->print();
    		rc.push_back(it->second);
		}
		for(int i=0;i<rc.size();i++){
			for(int j=0;j<rc.size();j++){
				if(rc[i]->id < rc[j]->id){
					Record_global* r;
					r=rc[i];
					rc[i]=rc[j];
					rc[j]=r;
				}
			}
		}
		for(int i=0;i<rc.size();i++){
			rc[i]->print();
		}
	}

	SymTable_local::SymTable_local(){
		id=0;
		this->locals_size=0;
	}
	void SymTable_local::add(string record_name,Record_local* record){
		record->id=id;
		id++;
		if(record->is_param){
			this->params.push_back(record);
		}
		this->symlist.insert(std::pair<string,Record_local*>(record_name,record));
		if(!record->is_param)
			this->locals_size = record->offset;
	}

	// void SymTable_local::getLocalSize(){
	// 	for (it=symlist.begin(); it!=symlist.end(); ++it){
 //    		if(it->second)
 //    		rc.push_back(it->second);
	// 	}		
	// }
	void SymTable_local::print(){
   		std::map<string,Record_local*>::iterator it = symlist.begin();
   		vector<Record_local*> rc;
   		cout<<"id\tname\tSF\tsize\toffset\ttype"<<endl;
		for (it=symlist.begin(); it!=symlist.end(); ++it){
    		rc.push_back(it->second);
		}
		for(int i=0;i<rc.size();i++){
			for(int j=0;j<rc.size();j++){
				if(rc[i]->id < rc[j]->id){
					Record_local* r;
					r=rc[i];
					rc[i]=rc[j];
					rc[j]=r;
				}
			}
		}
		for(int i=0;i<rc.size();i++){
			rc[i]->print();
		}

	}

	int SymTable_local::params_size(){
		int res=0;
		for(int i=0;i<params.size();i++){
			res += params[i]->size;
		}
		return res;
	}


	void SymTable_local::populate_offset(int num){

   		std::map<string,Record_local*>::iterator it = symlist.begin();
		for (it=symlist.begin(); it!=symlist.end(); ++it){
			it->second->offset+=num;
		}		
	}

	// int SymTable_local::locals_size(){

	// };

SymTable_global* gst = new SymTable_global();
SymTable_local* lst=0;
Type* type;
int width;
Type* old_type;
int old_width;
Record_global* grc;
Record_global* grc_temp;
Record_local* lrc;
Type*  current_return_type;
string symbol_name;
int struct_width =0;
bool is_param = true;
int offset;
int size_ebp=4;
bool is_intconst;
int val;
int symtree=1;
Type* Type_float = new BaseType("float");
Type* Type_int = new BaseType("int");int line=1;
Type* Type_void = new BaseType("void");
Type* Type_voidstar = new PointerType(new BaseType("void"));
int return_size;
string fun_name;

bool debugger = 0;
	Record_global* is_global(string id,sf_enum sf){
		if(gst->symlist.find(id)!=gst->symlist.end()){
			if(gst->symlist.find(id)->second->sf==sf)
				return gst->symlist.find(id)->second;
		}
		return NULL;
	}

vector<StringObject> global_strings;
// int fun(int a[10]){
// 	for(int i=0;i<15;i++){
// 		cout<<a[i]<<endl;
// 	}
// }
	
// int main(){
// 	Type* INT = new BaseType("int");
// 	Type* FLOAT = new BaseType("float");
// 	Type* STRUCT = new BaseType(("struct"));
	
// 	Type* INTSTAR = new PointerType(INT);
// 	Type* FLOATSTAR = new PointerType(FLOAT);
// 	Type* STRUCTSTAR = new PointerType(STRUCT);
	
// 	Type* INTARRAY = new ArrayType(10,INT);
// 	Type* INTARRAYARRAY = new ArrayType(10,INTARRAY);
// 	Type* INTAARRAY = new ArrayType(5,INTARRAY);
// 	INTAARRAY->print();
// 	INTARRAYARRAY->print();
	
// 	Type* INTSTARSTAR = new PointerType(INTSTAR);
	
// 	cout<<"INT INT"<<INTAARRAY->equal(INTSTARSTAR,0)<<endl;
// 	cout<<"INT INT"<<INTAARRAY->equal(INTARRAYARRAY,0)<<endl;
// 	cout<<"INT FLOAT"<<INTARRAY->equal(INTSTAR,0)<<endl;
// 	cout<<"INT FLOAT"<<INTSTAR->equal(INTARRAY)<<endl;
	
// 	// int x[5][10];
// 	// int a[15];
// 	// for(int i=0;i<15;i++){
// 	// 	a[i]=i;
// 	// }
// 	// int *b;
// 	// int **d;
// 	// fun(a);
	
// 	int (*a)[10];
// 	int b[10][10];
// 	int c[10][10];
// 	a=b;
// 	b=c;
// 	// fun(a,a,7,d);
	
// }