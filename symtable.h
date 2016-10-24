#ifndef SYMTABLE_H
#define SYMTABLE_H 

#include <iostream>	
#include <vector>
#include <string>
#include <map>
// #include 

using namespace std;

enum pv_enum{
	PARAM = 116,
	VAR = 117,
};

enum sf_enum{
	STRUCT = 119,
	FUN = 120,
};


class Type{
public:
	int dimension;
	string name;
	Type* type;
	string pab;
	virtual void print()=0;
	virtual Type* Dereference()=0;
	virtual bool equal(Type* type2,int rec=0)=0;
	virtual int size()=0;
};

class BaseType: public Type{
public:
	BaseType();
	BaseType(string name);
	void print();
	Type* Dereference();
	bool equal(Type* type2,int rec=0);
	int size();
	// Typse* ArrayDeref();
}; //TODO //type should be 0

class PointerType: public Type{
public:
	PointerType();
	PointerType(Type* type);
	void print();
	Type* Dereference();
	bool equal(Type* type2,int rec=0);
	int size();
	// Type* ArrayDeref();
};

class ArrayType: public Type{
public:
	ArrayType();
	ArrayType(int dimension, Type* type);
	bool equal(Type* type2,int rec=0);
	void print();
	int size();
	// Type* ArrayDeref();
	Type* Dereference();

};

class Record_local;

class SymTable_local;
class Record_global{
public:
	string symbol_name;
	sf_enum sf;
	Type* type;
	int size;
	SymTable_local *symtable;
	int id;

	Record_global();
	Record_global(
		string symbol_name,
		sf_enum sf,
		Type* type,
		int size,
		SymTable_local* symtable
	);
	vector<Record_local*> params();
	void print();
};

class Record_local{
public:
	string symbol_name;
	// pv_enum pv;
	bool is_param;	
	Type* type;
	int size;
	int offset;
	int id;

	Record_local();
	Record_local(
		string symbol_name,
		bool is_param,
		// pv_enum pv,
		Type* type,
		int size,
		int offset
	);
	void print();
};


class SymTable_global{
public:
	map<string,Record_global*> symlist;
	int id;
	SymTable_global();
	void add(string record_name,Record_global* record);
	void print();
};

class SymTable_local{
public:
	map<string,Record_local*> symlist;
	int id;
	vector<Record_local*> params;
	SymTable_local();
	void add(string record_name,Record_local* record);
	void populate_offset(int num);
	void print();
	int locals_size;
	int params_size();

};

class StringObject{
public:
	string str;
	string label;
};
extern vector<StringObject> global_strings;
Record_global* is_global(string id,sf_enum sf);

extern SymTable_global* gst;
extern SymTable_local* lst;
extern Type* type;
extern int width;
extern Type* old_type;
extern int old_width;
extern Record_global* grc;
extern Record_global* grc_temp;
extern Record_local* lrc;
extern string symbol_name;
extern int struct_width; //widht of struct
extern bool is_param;
extern int offset; 
extern int size_ebp;
extern bool is_intconst;
extern int val;
extern bool debugger;
extern int symtree;
extern Type* Type_int;
extern Type* Type_float;
extern Type*  current_return_type;
extern Type* Type_void;
extern Type* Type_voidstar;
extern int return_size;
extern string fun_name;
#endif