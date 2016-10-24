#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include "symtable.h"

using namespace std;


enum OPBinaryNames{
			 OR=1,
			 AND=2,
			 EQ_OP=3,
			 NE_OP=4,
			 LT=5,
			 	LT_FLOAT=6,
			 	LT_INT=7,
			 LE_OP=8,
			 	LE_OP_FLOAT=9,
			 	LE_OP_INT=10,
			 GT=11,
			 	GT_FLOAT=12,
			 	GT_INT=13,
			 GE_OP=14,
			 	GE_OP_FLOAT=15,
			 	GE_OP_INT=16,
			 PLUS=17,
			 	PLUS_FLOAT=18,
			 	PLUS_INT=19,
			 MINUS=20,
			 	MINUS_FLOAT=21,
			 	MINUS_INT=22,
			 MULT=23,
			 	MULT_FLOAT=24,
			 	MULT_INT=25,
			 DIV=26,
			 	DIV_FLOAT=27,
			 	DIV_INT=28,
};


enum OPUnaryNames{
			 UMINUS=30,
			 NOT=31,
			 PP=32,
			 ADDRESS=33,
			 STAR =34,
};




class abstract_astnode
{
public:
	Type* type;
	bool l_value;
	abstract_astnode();
	Record_global* function_record;
	virtual void print () = 0;
	virtual void generate_code(SymTable_local*, int=0) = 0;
	Type* getType();
	void setType(Type* type);
	
	protected:
};

class StmtAst: public abstract_astnode{
public:
	string nextlabel;
	virtual void print () = 0;
	virtual void generate_code(SymTable_local*, int=0) = 0;

};

class ExpAst: public StmtAst{ //pain aayega to baad me change karenge --charmi
public:
	bool left;
	ExpAst();
	virtual void print () = 0;
	virtual void generate_code(SymTable_local*, int=0) = 0;	
};


class Empty: public StmtAst{
public:
	Empty();
	void print();
	void generate_code(SymTable_local*, int=0);
};

class Seq: public StmtAst{
public:
	vector<StmtAst*> stmt_list;
	Seq();
	void addNode(StmtAst*);
	Seq(vector<StmtAst *> stmt_list);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class Ass : public StmtAst{
	ExpAst* expAst1;
	ExpAst* expAst2;
public:
	Ass();
	Ass(ExpAst* expAst1, ExpAst* expAst2);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class Return: public StmtAst{
	ExpAst* expAst;
public:
	Return();
	Return(ExpAst* expAst);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class If: public StmtAst{
	ExpAst * expAst;
	StmtAst* stmtAst1;
	StmtAst* stmtAst2;
public:
	If();
	void print();
	If(ExpAst* expAst, StmtAst* stmtAst1, StmtAst* stmtAst2);
	void generate_code(SymTable_local*, int=0);
};

class While: public StmtAst{
	ExpAst* expAst;
	StmtAst* stmtAst;
public:
	While();
	While(ExpAst* expAst, StmtAst* stmtAst);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class For: public StmtAst{
	ExpAst *expAst1, *expAst2, *expAst3;
	StmtAst* stmtAst;
public:
	For();
	For(ExpAst* expAst1,ExpAst* expAst2,ExpAst* expAst3,StmtAst* stmtAst);
	void print();
	void generate_code(SymTable_local*, int=0);	
};


class Op_binary: public ExpAst{
	OPBinaryNames op_name;
	ExpAst * expAst1, *expAst2;
public:
	Op_binary();
	Op_binary(OPBinaryNames);/*needed? later*/
	Op_binary(OPBinaryNames op_name, ExpAst* expAst1, ExpAst* expAst2);
	void SetChildren(ExpAst *expAst1, ExpAst* expAst2);/**/
	void print();
	void generate_code(SymTable_local*, int=0);
};

class Op_unary: public ExpAst{
	OPUnaryNames op_name;
	ExpAst* expAst;
public:
	Op_unary();
	// Op_unary(OPUnaryNames op_name);
	void SetChild(ExpAst * expAst);
	Op_unary(OPUnaryNames op_name, ExpAst* expAst);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class StringConst: public ExpAst{
	string stringValue;
public:
	StringConst();
	StringConst(string stringValue);
	void print();
	void generate_code(SymTable_local*, int=0);
};
class Identifier: public ExpAst{
public:
	string identifier;
	Identifier();
	Identifier(string identifier);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class Assign: public ExpAst{
	ExpAst* expAst1;
	ExpAst* expAst2;
public:
	Assign();
	Assign(ExpAst* expAst1, ExpAst* expAst2);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class Funcall: public ExpAst{
public:
	Identifier* identifier;
	std::vector<ExpAst*>* exp_list;
	Funcall();
	Funcall(Identifier * identifier);
	Funcall(Identifier* identifier, std::vector<ExpAst* >* exp_list);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class ToFloat: public ExpAst{
public:
	ExpAst *expAst;
	ToFloat();
	ToFloat(ExpAst *expAst);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class ToInt: public ExpAst{
	public:
	ExpAst *expAst;
	ToInt();
	ToInt(ExpAst *expAst);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class FloatConst: public ExpAst{
	float floatValue;
public:
	FloatConst();
	FloatConst(std::string floatValue);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class IntConst: public ExpAst{
	int intValue;
public:
	IntConst();
	IntConst(std::string intValue);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class Pointer: public ExpAst{
	ExpAst* expAst;
public:
	Pointer();
	Pointer(ExpAst*);
	void print();
	void generate_code(SymTable_local*, int=0);
};


class ArrayRef: public ExpAst{
	ExpAst* expAst1;
	ExpAst* expAst2;
public:
	ArrayRef();
	ArrayRef(ExpAst*, ExpAst*);
	void print();
	void generate_code(SymTable_local*, int=0);
};


class Deref: public ExpAst{
	ExpAst* expAst;
public:
	Deref();
	Deref(ExpAst* expAst);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class Member: public ExpAst{
	ExpAst* expAst;
	Identifier* identifier;
public:
	Member();
	Member(ExpAst* , Identifier*);
	void print();
	void generate_code(SymTable_local*, int=0);
};

class Arrow: public ExpAst{
	ExpAst* expAst;
	Identifier* identifier;
public:
	Arrow();
	Arrow(ExpAst* , Identifier*);
	void print();
	void generate_code(SymTable_local*, int=0);
};

void load_offset(int );
void stack_pop();
void stack_pop2();
void stack_push();
void float_push();
void float_pop();
void printf_float();
void printf_int();
void printf_string();
void printf_space();
void printf_endline();

string newlabel();
extern Seq printSequence;