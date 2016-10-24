#include "ast.h"
int num_tabs=0;
void tabs(int tab){
    for(int i=0;i<tab;i++){
        cout<<"\t";
    }
}

Seq printSequence;
std::string OP_TO_STRING[50]={
	"",
	"OR",
	"AND",
	"EQ_OP",
	"NE_OP",
	"LT",
	"LT-FLOAT",
	"LT-INT",
	"LE_OP",
	"LE_OP-FLOAT",
	"LE_OP-INT",
	"GT",
	"GT_FLOAT",
	"GT_INT",
	"GE_OP",
	"GE_OP_FLOAT",
	"GE_OP_INT",
	"PLUS",
	"PLUS_FLOAT",
	"PLUS_INT",
	"MINUS",
	"MINUS_FLOAT",
	"MINUS_INT",
	"MULT",
	"MULT_FLOAT",
	"MULT_INT",
	"DIV",
	"DIV_FLOAT",
	"DIV_INT",
	"",
	"UMINUS",
	"NOT",
	"PP",
	"ADDRESS ",
	"DEREF "
};


	abstract_astnode::abstract_astnode(){
		l_value=false;
	}

	ExpAst::ExpAst(){
		left=false;
	}

	Type* abstract_astnode::getType(){
		return type;
	}
	void abstract_astnode::setType(Type* type){
		this->type=type;
	}

	Empty::Empty(){

	}
    void Empty::print(){
        cout<<"(Empty)";
    }

	void Empty::generate_code(SymTable_local* symbol_table, int left){
	}


	ToFloat::ToFloat(){

	}
	ToFloat::ToFloat(ExpAst* expAst){
		this->expAst = expAst;
		this->type= Type_float;
	}
	void ToFloat::print(){
		cout<<"( TO-FLOAT";
		this->expAst->print();
		cout<<" )";
	}
	void ToFloat::generate_code(SymTable_local* symbol_table, int left){
		this->expAst->generate_code(symbol_table);
		float_pop();
		cout<<"cvt.s.w $f0 $f0"<<endl;
		float_push();
	}

	ToInt::ToInt(){}
	ToInt::ToInt(ExpAst* expAst){
		this->expAst = expAst;
		this->type=Type_int;
	}

	void ToInt::print(){
		cout<<"( TO-INT";
		this->expAst->print();
		cout<<" )";
	}
	void ToInt::generate_code(SymTable_local* symbol_table, int left){
		this->expAst->generate_code(symbol_table);
		float_pop();
		cout<<"cvt.w.s $f0 $f0"<<endl;
		float_push();
	}


	Seq::Seq(vector<StmtAst *> stmt_list){
	    this->stmt_list = stmt_list;
	}
	Seq::Seq(){
		
	}
	void Seq::generate_code(SymTable_local* symbol_table, int left){
		for(int i=0;i<stmt_list.size()-1;i++){
			// string s = newlabel();

			// stmt_list[i]->nextlabel=s;
			stmt_list[i]->generate_code(symbol_table);
			// cout<<s<<":"<<endl;
			stack_pop();
		}
		// stmt_list.back()->nextlabel = this->nextlabel;
		stmt_list.back()->generate_code(symbol_table);
	}
	void Seq::addNode(StmtAst* stmt){
		this->stmt_list.push_back(stmt);
	}
	void Seq::print(){
	    cout<<"(Block [";
        num_tabs++;
	    for(int i=0; i<stmt_list.size();i++){
	        stmt_list[i]->print();
	        if(i!=stmt_list.size()-1){
		        cout<<"\n";
		        tabs(num_tabs);
		    }
	    }
        num_tabs--;
	    cout<<"])";
	}


	Ass::Ass(){
		
	}
	Ass::Ass(ExpAst* expAst1, ExpAst* expAst2){
	    this->expAst1 = expAst1;
	    this->expAst2 = expAst2;
	    
	}
	void Ass::print(){
	    cout<<"(Ass ";
	    // num_tabs++;
	    expAst1->print();
	    cout<<" ";
	    expAst2->print();
	    // num_tabs--;
	    cout<<")";
	}
	void Ass::generate_code(SymTable_local* symbol_table, int left){
	}

	Return::Return(){
		
	}
	Return::Return(ExpAst* expAst){
	    this->expAst = expAst;
	}
	void Return::print(){
	    cout<<"(Return ";
	    // num_tabs++;
	    expAst->print();
	    // num_tabs--;
	    cout<<")";
	}
	void Return::generate_code(SymTable_local* symbol_table, int left){
		// this->expAst->generate_code(symbol_table);

		cout<<"#Return"<<endl;
		// this->expAst1->generate_code(symbol_table,true);
		// cout<<"#left done"<<endl;
		cout<<"#";
		this->expAst->type->print();
		// cout<<endl;
		cout<<endl;
		if(this->expAst->type->equal(Type_int)|| this->expAst->type->pab=="pointer"){
			cout<<"# int / float/ pointer"<<endl;
			this->expAst->generate_code(symbol_table);
			cout<<"#generated"<<endl;
			stack_pop();
			cout<<"#restore fp"<<endl;
            cout<<"addi $sp $fp 0"<<endl;
            cout<<"lw $fp ($fp)"<<endl;
            cout<<"addi $sp $sp 4"<<endl<<endl;

            cout<<"lw $ra ($sp)"<<endl;
            cout<<"addi $sp $sp 4"<<endl;

            cout<<"#skip params"<<endl;
            cout<<"addi $sp $sp "<<symbol_table->params_size()<<endl;
            cout<<"sw $s1 ($sp)"<<endl;

            cout<<"jr $ra"<<endl;
		}
		else{
			cout<<"#other types"<<endl;
			this->expAst->generate_code(symbol_table,true);			
			cout<<"#right address done "<<endl;
			stack_pop();
			int size = this->expAst->type->size();
			cout<<"#restore fp"<<endl;
            cout<<"addi $sp $fp 0"<<endl;
            cout<<"lw $fp ($fp)"<<endl;
            cout<<"addi $sp $sp 4"<<endl<<endl;

            cout<<"lw $ra ($sp)"<<endl;
            cout<<"addi $sp $sp 4"<<endl;

            cout<<"#skip params"<<endl;
            cout<<"addi $sp $sp "<<symbol_table->params_size()<<endl;

			for(int i=0;i<size;i=i+4){
				cout<<"lw $t3 "<<i<<"($s1)"<<endl;
				cout<<"sw $t3 "<<i<<"($sp)"<<endl;
			}
			cout<<"#Struct copied to stack"<<endl;
			cout<<"jr $ra"<<endl;
		}//mostly done

	}

	If::If(){
	}
    If::If(ExpAst* expAst, StmtAst* stmtAst1, StmtAst* stmtAst2){
        this->expAst = expAst;
        this->stmtAst1 = stmtAst1;
        this->stmtAst2 = stmtAst2;
    }
    
    void If::print(){
	    cout<<"(If ";
	    num_tabs++;
        expAst->print();
        cout<<"\n";
        tabs(num_tabs);
        stmtAst1->print();
        cout<<"\n";
        tabs(num_tabs);
        stmtAst2->print();
        num_tabs--;
	    cout<<")";
    }
  	void If::generate_code(SymTable_local* symbol_table, int left){
  		string elselabel= newlabel();
  		string endlabel= newlabel();
  		// string l2 = newlabel();

  		cout<<"\t\t# if begins"<<endl;
  		this->expAst->generate_code(symbol_table);
  		stack_pop();
  		cout<<"beq $s1 $0 "<<elselabel<<endl;//if
  		this->stmtAst1->generate_code(symbol_table); // if
  		cout<<"j "<<endlabel <<endl;
  		
  		cout<<elselabel<<":"<<endl;
  		this->stmtAst2->generate_code(symbol_table); // else
  		cout<<endlabel<<":"<<endl;
  		cout<<"\t\t#if ends"<<endl;

	}


	While::While(){
		
	}
	
	While::While(ExpAst* expAst, StmtAst* stmtAst){
	    this->expAst = expAst;
	    this->stmtAst = stmtAst;
	}
	
	void While::print(){
	    cout<<"(While ";
	    num_tabs++;
	    expAst->print();
	    cout<<"\n";
	    tabs(num_tabs);
	    stmtAst->print();
	    num_tabs--;
	    cout<<")";
	}
	void While::generate_code(SymTable_local* symbol_table, int left){
		cout<<"\t#while cond"<<endl;
		string cond = newlabel();
		string endwhile = newlabel();
		cout<<cond<<":"<<endl;
		this->expAst->generate_code(symbol_table);
		stack_pop();
		cout<<"beq $s1 $0 "<<endwhile;

		cout<<"\t#body"<<endl;
		// this->stmtAst->nextlabel = cond;
		this->stmtAst->generate_code(symbol_table);
		cout<<"j "<<cond<<endl;
		cout<<endwhile<<":"<<endl;
		cout<<"\t#end while"<<endl;

	}

	For::For(){
		
	}
	
	For::For(ExpAst* expAst1,ExpAst* expAst2,ExpAst* expAst3,StmtAst* stmtAst){
	    this->expAst1 = expAst1;
	    this->expAst2 = expAst2;
	    this->expAst3 = expAst3;
	    this->stmtAst = stmtAst;
	}
	void For::print(){
	    cout<<"(For ";
	    num_tabs++;
	    expAst1->print();
	    cout<<"\n";
	    tabs(num_tabs);
	    expAst2->print();
	    cout<<"\n";
	    tabs(num_tabs);
	    expAst3->print();
	    cout<<"\n";
	    tabs(num_tabs);
	    stmtAst->print();
	    num_tabs--;
	    cout<<")";
	}
	void For::generate_code(SymTable_local* symbol_table, int left){
		cout<<"\t#for init"<<endl;
		this->expAst1->generate_code(symbol_table);
		string e2=newlabel();
		string e3 = newlabel();
		string s = newlabel();
		string endfor = newlabel();

		cout<<"\t#check condition"<<endl;
		cout<<e2<<":"<<endl;
		this->expAst2->generate_code(symbol_table);
		stack_pop();
		cout<<"beq $s1 $0 "<<endfor<<endl;
		// cout<<

		cout<<"\t#loop body"<<endl;
		// this->stmtAst->nextlabel=e3;
		this->stmtAst->generate_code(symbol_table);
		cout<<"\t#increment"<<endl;
		cout<<e3<<":"<<endl;
		this->expAst3->generate_code(symbol_table);
		cout<<"j "<<e2<<endl;

		cout<<endfor<<":"<<endl;

	}


	Op_binary::Op_binary(){
		
	}

	Op_binary::Op_binary(OPBinaryNames op_name, ExpAst* expAst1, ExpAst* expAst2){
		this->op_name=op_name;
		this->expAst1=expAst1;
		this->expAst2=expAst2;
	}
	Op_binary::Op_binary(OPBinaryNames op_name){
		this->op_name = op_name;
	}
	void Op_binary::SetChildren(ExpAst *expAst1, ExpAst* expAst2){
		this->expAst1 = expAst1;
		this->expAst2 = expAst2;
	}
    void Op_binary::print(){
	    cout<<"("<<OP_TO_STRING[op_name]<<" ";
	    // num_tabs++;
        expAst1->print();
        cout<<" ";
        expAst2->print();
        // num_tabs--;
       cout<<")";
    }
	void Op_binary::generate_code(SymTable_local* symbol_table, int left){
			string label;
			string nlabel;
			string nlabel2;
			//lazy evaluation
	    	if(!(op_name==OPBinaryNames::OR || op_name==OPBinaryNames::AND)){
		    	this->expAst1->generate_code(symbol_table);
		    	this->expAst2->generate_code(symbol_table);
	    		stack_pop2();
	    	}

	    	switch(op_name){
	    		case OPBinaryNames::OR:
	    			nlabel = newlabel();
	    			nlabel2 = newlabel();
	    			this->expAst1->generate_code(symbol_table);
	    			stack_pop();
					cout<<"bne $s1 $0 "<<nlabel<<endl;
	    			this->expAst2->generate_code(symbol_table);					
					stack_pop();
					cout<<"bne $s1 $0 "<<nlabel<<endl;
					cout<<"li $s1 0"<<endl;
					cout<<"j "<<nlabel2<<endl;
	    			cout<<nlabel<<":"<<endl;
	    			cout<<"li $s1 1"<<endl;
					cout<<nlabel2<<":"<<endl;
	    			break;
	    		case OPBinaryNames::AND: 	
	    			nlabel = newlabel();
	    			nlabel2 = newlabel();
	    			this->expAst1->generate_code(symbol_table);					
					stack_pop();
					cout<<"beq $s1 $0 "<<nlabel<<endl;
	    			this->expAst2->generate_code(symbol_table);
	    			stack_pop();					
					cout<<"beq $s1 $0 "<<nlabel<<endl;
					cout<<"li $s1 1"<<endl;
					cout<<"j "<<nlabel2<<endl;
	    			cout<<nlabel<<":"<<endl;
	    			cout<<"li $s1 0"<<endl;
					cout<<nlabel2<<":"<<endl;
	    			break;
	    		case OPBinaryNames::EQ_OP:
					cout<<"xor $s1 $s1 $s2"<<endl;
					cout<<"sltiu $s1 $s1 1"<<endl;
	    			break;
	    		case OPBinaryNames::NE_OP:
					cout<<"xor $s1 $s1 $s2"<<endl;
					cout<<"sltu $s1 $0 $s1"<<endl;
	    			break;	    		
	    		case OPBinaryNames::LT:
	    			cout<<"slt $s1 $s1 $s2"<<endl;
	    			break;
	    		case OPBinaryNames::LT_FLOAT:
	    			cout<<"mtc1 $s1 $f0"<<endl;
	    			cout<<"mtc1 $s2 $f1"<<endl;
	    			// cout<<"slt $s1 $s1 $s2"<<endl;
	    			cout<<"c.lt.s $f0, $f1"<<endl;
	    			label=newlabel();
	    			// label1=newlabel();
	    			cout<<"li $s1 1"<<endl;
	    			cout<<"bc1t "<<label<<endl;
	    			cout<<"li $s1 0"<<endl;
	    			cout<<label<<":"<<endl;
	    			break;
	    		case OPBinaryNames::LT_INT://change karna hai
	    			cout<<"slt $s1 $s1 $s2"<<endl;
	    			break;
	    		case OPBinaryNames::LE_OP:
	    			cout<<"slt $s1 $s2 $s1"<<endl;
	    			cout<<"xori $s1 $s1 0x1"<<endl;
					break;
	    		case OPBinaryNames::LE_OP_FLOAT:
	    			cout<<"mtc1 $s1 $f0"<<endl;
	    			cout<<"mtc1 $s2 $f1"<<endl;
	    			// cout<<"slt $s1 $s1 $s2"<<endl;
	    			cout<<"c.le.s $f0, $f1"<<endl;
	    			label=newlabel();
	    			// label1=newlabel();
	    			cout<<"li $s1 1"<<endl;
	    			cout<<"bc1t "<<label<<endl;
	    			cout<<"li $s1 0"<<endl;
	    			cout<<label<<":"<<endl;
	    			break;
	    		case OPBinaryNames::LE_OP_INT:
	    			cout<<"slt $s1 $s2 $s1"<<endl;
	    			cout<<"xori $s1 $s1 0x1"<<endl;
					break;
	    		case OPBinaryNames::GT:
	    			cout<<"slt $s1 $s2 $s1"<<endl;
	    			break;
	    		case OPBinaryNames::GT_FLOAT:
	    			cout<<"#gt float"<<endl;
	    			cout<<"mtc1 $s1 $f0"<<endl;
	    			cout<<"mtc1 $s2 $f1"<<endl;
	    			// cout<<"slt $s1 $s1 $s2"<<endl;
	    			cout<<"c.lt.s $f1, $f0"<<endl;
	    			label=newlabel();
	    			// label1=newlabel();
	    			cout<<"li $s1 1"<<endl;
	    			cout<<"bc1t "<<label<<endl;
	    			cout<<"li $s1 0"<<endl;
	    			cout<<label<<":"<<endl;
	    			break;
	    		case OPBinaryNames::GT_INT://change karna hai
	    			cout<<"slt $s1 $s2 $s1"<<endl;
	    			break;
	    		case OPBinaryNames::GE_OP:
	    			cout<<"slt $s1 $s1 $s2"<<endl;
	    			cout<<"xori $s1 $s1 0x1"<<endl;
					break;
	    		case OPBinaryNames::GE_OP_FLOAT:
	    			cout<<"#fgt float"<<endl;
	    			cout<<"mtc1 $s1 $f0"<<endl;
	    			cout<<"mtc1 $s2 $f1"<<endl;
	    			// cout<<"slt $s1 $s1 $s2"<<endl;
	    			cout<<"c.le.s $f1, $f0"<<endl;
	    			label=newlabel();
	    			// label1=newlabel();
	    			cout<<"li $s1 1"<<endl;
	    			cout<<"bc1t "<<label<<endl;
	    			cout<<"li $s1 0"<<endl;
	    			cout<<label<<":"<<endl;
	    			break;
	    		case OPBinaryNames::GE_OP_INT:

	    			cout<<"slt $s1 $s1 $s2"<<endl;
	    			cout<<"xori $s1 $s1 0x1"<<endl;
					break;
				case OPBinaryNames::PLUS:
					cout<<"add $s1 $s1 $s2"<<endl;
					break;
				case OPBinaryNames::PLUS_FLOAT:
					cout<<"mtc1 $s1 $f0"<<endl;
	    			cout<<"mtc1 $s2 $f1"<<endl;
					cout<<"add.s $f0 $f0 $f1"<<endl;
					cout<<"mfc1 $s1 $f0"<<endl;
					break;
				case OPBinaryNames::PLUS_INT:
					cout<<"add $s1 $s1 $s2"<<endl;
					break;
				case OPBinaryNames::MINUS:
					cout<<"sub $s1 $s1 $s2"<<endl;
					break;
				case OPBinaryNames::MINUS_FLOAT:
					cout<<"mtc1 $s1 $f0"<<endl;
	    			cout<<"mtc1 $s2 $f1"<<endl;
					cout<<"sub.s $f0 $f0 $f1"<<endl;
					cout<<"mfc1 $s1 $f0"<<endl;
					break;
				case OPBinaryNames::MINUS_INT:
					cout<<"sub $s1 $s1 $s2"<<endl;
					break;

				case OPBinaryNames::MULT_INT:
					cout<<"mult $s1 $s2"<<endl;
					cout<<"mflo $s1"<<endl;
					break;
				case OPBinaryNames::MULT_FLOAT:
					cout<<"mtc1 $s1 $f0"<<endl;
	    			cout<<"mtc1 $s2 $f1"<<endl;
					cout<<"mul.s $f0 $f0 $f1"<<endl;
					cout<<"mfc1 $s1 $f0"<<endl;
					break;
				case OPBinaryNames::DIV_INT:
					cout<<"div $s1 $s2"<<endl;
					cout<<"mflo $s1"<<endl;
					break;
				case OPBinaryNames::DIV_FLOAT:
					cout<<"mtc1 $s1 $f0"<<endl;
	    			cout<<"mtc1 $s2 $f1"<<endl;
					cout<<"div.s $f0 $f0 $f1"<<endl;
					cout<<"mfc1 $s1 $f0"<<endl;
					break;
				default:
					break;
	    	}
		stack_push();
	}
    

	Op_unary::Op_unary(){
					
	}

	void Op_unary::SetChild(ExpAst *expAst){
		this->expAst = expAst;
	}
	Op_unary::Op_unary(OPUnaryNames op_name1, ExpAst* expAst){
	    this->op_name=op_name1;
	    this->expAst=expAst;
	}
    void Op_unary::print(){
	    cout<<"("<<OP_TO_STRING[op_name]<<" ";
        expAst->print();
        cout<<")";
    }

	void Op_unary::generate_code(SymTable_local* symbol_table, int left){
    	if(op_name==OPUnaryNames::UMINUS){
	    	//expAst->left = false;
	    	this->expAst->generate_code(symbol_table);
	    	cout<<"lw $s1 ($sp)"<<endl;
	    	cout<<"li $t7 -1"<<endl;
	    	cout<<"mul $s1 $s1 $t7"<<endl;
	    	cout<<"sw $s1 ($sp)"<<endl;
	    }
	    else if(op_name==OPUnaryNames::PP){
	    	this->expAst->generate_code(symbol_table,true);
	    	cout<<"lw $s1 ($sp)"<<endl;
	    	cout<<"lw $s2 ($s1)"<<endl;
	    	cout<<"lw $t1 ($s1)"<<endl;
	    	cout<<"addi $s2 $s2 1"<<endl;
	    	cout<<"sw $t1 ($sp)"<<endl;
	    	cout<<"sw $s2 ($s1)"<<endl;
	    }
	    else if(op_name==OPUnaryNames::NOT){
	    	expAst->generate_code(symbol_table);
	    	string label = newlabel();
	    	string label2 = newlabel();
	    	cout<<"lw $s1 ($sp)"<<endl;
	    	cout<<"bne $s1 $0 "<<label<<endl;
	    	cout<<"addi $s1 $0 1"<<endl;
	    	cout<<"j "<<label2<<endl;
	    	cout<<endl;
	    	cout<<label<<":"<<endl;
	    	cout<<"addi $s1 $0 0"<<endl;
	    	cout<<endl;
	    	cout<<label2<<":"<<endl;
	    	cout<<"sw $s1 ($sp)"<<endl;
	    }
	    else if(op_name==OPUnaryNames::ADDRESS){
	    	expAst->generate_code(symbol_table,true);
	    }
	    else if(op_name==OPUnaryNames::STAR){
	    	if(expAst->type->pab=="array"){
	    		expAst->generate_code(symbol_table,true);
	    	}
	    	else
		    	expAst->generate_code(symbol_table);

	    	if (!left ){
		    	cout<<"lw $s1 ($sp)"<<endl;
	    		cout<<"lw $s1 ($s1)"<<endl;
		    	cout<<"sw $s1 ($sp)"<<endl;
	    	}	    	
	    }
    }

	Funcall::Funcall(){
		
	}
	Funcall::Funcall(Identifier * identifier){
		this->identifier = identifier;
		this->exp_list = new vector<ExpAst*>;
	}

	
	Funcall::Funcall(Identifier* identifier, std::vector<ExpAst*>* exp_list){
	    this->identifier = identifier;
	    this->exp_list=exp_list;
	}

    void Funcall::print(){
	    cout<<"(Funcall [ ";
	    num_tabs++;
        for(int i=0;i<exp_list->size();i++){
            (*(exp_list))[i]->print();
	        if(i!=exp_list->size()-1){
            	cout<<"\n";
		        tabs(num_tabs);
		    }
        }
        num_tabs--;
        cout<<"])";
    }
	void Funcall::generate_code(SymTable_local* symbol_table, int left){
		if(identifier->identifier=="printf"){
			for(int i=0;i< exp_list->size();i++){
				if((*exp_list)[i]->type->pab=="array"){
					(*exp_list)[i]->generate_code(symbol_table,true);
				}
				else
					(*exp_list)[i]->generate_code(symbol_table);
				if((*exp_list)[i]->type->name=="int" || (*exp_list)[i]->type->pab=="pointer" || (*exp_list)[i]->type->pab=="array"){
					printf_int();
				}
				if((*exp_list)[i]->type->name=="float"){
					printf_float();
				}
				if((*exp_list)[i]->type->name=="string"){
					printf_string();
				}
				printf_space();
			}
			printf_endline();
		}
		else{
			cout<<"#Funcall "<<identifier->identifier<<endl;
			cout<<"addi $sp $sp -"<<this->type->size()<<endl;//space for return value
			for(int i=0; i< exp_list->size();i++){

				int size = (*exp_list)[i]->type->size();
				cout<<"#param "<<i <<" size "<<size<< endl;

				if((*exp_list)[i]->type->pab=="array"){
					cout<<"#array pointer passed"<<endl;
					(*exp_list)[i]->generate_code(symbol_table,true);//array pointer passed
				}
				else if((*exp_list)[i]->type->pab=="pointer" || (*exp_list)[i]->type->equal(Type_int) ){
					cout<<"#integer/pointer passed by value"<<endl;
					(*exp_list)[i]->generate_code(symbol_table);
				}
				else{
					cout<<"#struct "<<endl;
					(*exp_list)[i]->generate_code(symbol_table,true);
					stack_pop();
					cout<<"addi $sp $sp -"<<size<<endl;//make space
					for(int i=0;i<size;i=i+4){
						cout<<"lw $t3 "<<i<<"($s1)"<<endl;
						cout<<"sw $t3 "<<i<<"($sp)"<<endl;
					}
					cout<<"#struct copied"<<endl;
				}
			}
			cout<<"#params generated"<<endl;
			cout<<"jal "<<identifier->identifier<<endl;
			int my_size = this->type->size();
			cout<<"#pick return value"<<endl;
			if(!left){
				cout<<"#no need , not left"<<endl;
				// stack_pop();
				cout<<"addi $sp $sp "<<my_size-4<<endl;
				// stack_push();
				//return value on top of stack
			}
			else{//
				cout<<"#must be struct ? top of stack"<<endl;
				cout<<"addi $s1 $sp -4"<<endl;
				for(int i=0;i<my_size;i=i+4){
					cout<<"lw $t5 "<<i<<"($sp)"<<endl;
					cout<<"sw $t5 "<<i-4<<"($sp)"<<endl;
				}
				cout<<"addi $sp $sp "<<my_size<<endl;

				stack_push();
			}
		}
	}

	void printf_float(){
		cout<<"lwc1 $f12 ($sp)"<<endl;
		cout<<"addi $sp $sp 4"<<endl;
		cout<<"li $v0 2"<<endl;
		cout<<"syscall"<<endl;
	}


	void printf_int(){
		cout<<"lw $a0 ($sp)"<<endl;
		cout<<"addi $sp $sp 4"<<endl;
		cout<<"li $v0 1"<<endl;
		cout<<"syscall"<<endl;
	}
    
    void printf_string(){
		cout<<"lw $a0 ($sp)"<<endl;
		cout<<"addi $sp $sp 4"<<endl;
		cout<<"li $v0 4"<<endl;
		cout<<"syscall"<<endl;
    }

    void printf_space(){
		cout<<"la $a0 SPACE"<<endl;
		cout<<"li $v0 4"<<endl;
		cout<<"syscall"<<endl;
    }

    void printf_endline(){
		cout<<"la $a0 ENDLINE"<<endl;
		cout<<"li $v0 4"<<endl;
		cout<<"syscall"<<endl;
    }



    FloatConst::FloatConst(){
    	
    }
    
	FloatConst::FloatConst(std::string floatValue){
	    this->floatValue=atof(floatValue.c_str());
	}
	void FloatConst::print(){
	    cout<<"(FloatConst "<<floatValue<<")";
	}
	void FloatConst::generate_code(SymTable_local* symbol_table, int left){
		cout<<"li.s $f1 "<<to_string(floatValue)<<endl;
		cout<<"addi $sp $sp -4"<<endl;
		cout<<"swc1 $f1 ($sp)"<<endl;	}

	IntConst::IntConst(){
		
	}
	
	IntConst::IntConst(std::string intValue){
	    this->intValue=atoi(intValue.c_str());
	}
	
    void IntConst::print(){
	    cout<<"(IntConst "<<intValue<<")";
    }
	void IntConst::generate_code(SymTable_local* symbol_table, int left){
		cout<<"li $s1 "<<intValue<<endl;
		stack_push();
	}
    
    StringConst::StringConst(){
    	
    }
    
	StringConst::StringConst(string stringValue){
	    this->stringValue=stringValue;
	}
    
    void StringConst::print(){
	    cout<<"(StringConst "<<stringValue<<")";
    }
	void StringConst::generate_code(SymTable_local* symbol_table, int left){
		cout<<"la $s1 "<<stringValue<<endl;
		stack_push();	
	}

	Identifier::Identifier(){
		
	}

	Identifier::Identifier(string identifier){
	    this->identifier=identifier;
	}
	
	void Identifier::print(){
	    cout<<"(Id ''"<<identifier<<"'')";
	}
	void Identifier::generate_code(SymTable_local* symbol_table, int left){
		int var_off = symbol_table->symlist.find(identifier)->second->offset;
		if(!left)
			load_offset(var_off);
		else
			cout<<"addi $s1 $fp "<<var_off<<endl;
		stack_push();
	}

	void load_offset(int off){
		cout<<"lw $s1 "<<off<<"($fp)"<<endl;		
	}
	void stack_push(){
		cout<<"addi $sp $sp -4"<<endl;
		cout<<"sw $s1 ($sp)"<<endl;
	}
	
	void stack_pop2(){
		cout<<"lw $s2 ($sp)"<<endl;
		cout<<"lw $s1 4($sp)"<<endl;
		cout<<"addi $sp $sp 8"<<endl;
	}
	void stack_pop(){
		cout<<"lw $s1 ($sp)"<<endl;
		cout<<"addi $sp $sp 4"<<endl;
	}

	void float_pop(){
		cout<<"lwc1 $f0, ($sp)"<<endl;
		cout<<"addi $sp $sp 4"<<endl;
	}

	void float_push(){
		cout<<"addi $sp $sp -4"<<endl;
		cout<<"swc1 $f0 ($sp)"<<endl;
	}
	Assign::Assign(){
	}

	Assign::Assign(ExpAst* expAst1, ExpAst* expAst2){
		this->expAst1=expAst1;
		this->expAst2=expAst2;
	}

	void Assign::print(){
		cout<<"(Ass ";
		expAst1->print();
		cout<<" ";
		expAst2->print();
		cout<<")";
	}
	void Assign::generate_code(SymTable_local* symbol_table, int left){
			//kchh to karna hai
		//void * ?
		cout<<"#Assign"<<endl;


		this->expAst1->generate_code(symbol_table,true);
		cout<<"#left done"<<endl;
		// cout<<
		// this->expAst1->type->print();
		cout<<endl;
		// this->expAst2->type->print();
		cout<<endl;
		if(this->expAst1->type->equal(Type_int)){
			cout<<"#lhs int / float"<<endl;
			this->expAst2->generate_code(symbol_table);
			cout<<"#"<<endl;
			cout<<"#right done"<<endl;
			stack_pop2();
			cout<<"#stack_pop2"<<endl;
			cout<<"sw $s2 ($s1)"<<endl;
			cout<<"move $s1 $s2 #value of assign moved to s1 so that pushed to stack"<<endl;
			stack_push();
			cout<<"#Assign Done. Value copied"<<endl;
		}
		else if(this->expAst1->type->pab=="pointer"){
			if(this->expAst2->type->pab=="pointer"){
				cout<<"#lhs pointer, rhs pointer"<<endl;
				this->expAst2->generate_code(symbol_table);
				cout<<"#"<<endl;
				cout<<"#right done"<<endl;
				stack_pop2();
				cout<<"#stack_pop2"<<endl;
				cout<<"sw $s2 ($s1)"<<endl;
				cout<<"move $s1 $s2 #value of assign moved to s1 so that pushed to stack"<<endl;
				stack_push();
				cout<<"#Assign Done. Value copied"<<endl;
			}
			if(this->expAst2->type->pab=="array"){
				cout<<"#lhs pointer, rhs array"<<endl;
				this->expAst2->generate_code(symbol_table,true);
				cout<<"#"<<endl;
				cout<<"#right done"<<endl;
				stack_pop2();
				cout<<"#stack_pop2"<<endl;
				cout<<"sw $s2 ($s1)"<<endl;
				cout<<"move $s1 $s2 #value of assign moved to s1 so that pushed to stack"<<endl;
				stack_push();
				cout<<"#Assign Done. Value copied"<<endl;
			}
		}
		else{
			cout<<"#other types"<<endl;
			this->expAst2->generate_code(symbol_table,true);			
			cout<<"#right address done "<<endl;
			stack_pop2();
			int size = this->expAst1->type->size();
			cerr<<size<<endl;
			for(int i=0;i<size;i=i+4){
				cout<<"lw $t3 "<<i<<"($s2)"<<endl;
				cout<<"sw $t3 "<<i<<"($s1)"<<endl;
			}
			stack_push(); //s1 ko push karenge ki s2 ko ?
			cout<<"#Assign done. Struct copied"<<endl;
		}//mostly done
	}

	ArrayRef::ArrayRef(){
		
	}

	ArrayRef::ArrayRef(ExpAst* expAst1, ExpAst* expAst2){
		this->expAst1 = expAst1;
		this->expAst2 = expAst2;

	}

    void ArrayRef::print(){
        cout<<"(ArrayRef ";
        expAst1->print();
        cout<<" ";
        expAst2->print();
        cout<<")";
    }
	void ArrayRef::generate_code(SymTable_local* symbol_table, int left){
		cout<<"#ArrayRef"<<endl;
		if(this->expAst1->type->pab =="array")
			this->expAst1->generate_code(symbol_table,true);
		else
			this->expAst1->generate_code(symbol_table,false);
		cout<<"#left done"<<endl;
		this->expAst2->generate_code(symbol_table);
		cout<<"#index done"<<endl;
		stack_pop2();
		int size = this->type->size(); //size of type TODO
		// cout<<"sll $s2 $s2 2"<<endl;//s2*4, index * size
		cout<<"li $t1 "<<size<<endl;
		cout<<"mult $s2 $t1"<<endl;
		cout<<"mflo $s2"<<endl;
		cout<<"add $s1 $s1 $s2"<<endl;

		if(!left) {//added this condition
			cout<<"#left false"<<endl;
			cout<<"lw $s1 ($s1)"<<endl;
		}
		stack_push();
		//galat hai
		//galat kya hai isme??
		//bahut kuchh
		//shayad sahi check nhi kiya
		// cout<<"# ArrayRef galat hai"<<endl;

	}



	Member::Member(){
	}

	Member::Member(ExpAst* expAst , Identifier* identifier){
		this->expAst=expAst;
		this->identifier=identifier;
	}
	void Member::print()
	{
		cout<<"(Member ";
		expAst->print();
		cout<<" ";
		identifier->print();
		cout<<")";
	}
	void Member::generate_code(SymTable_local* symbol_table, int left){
		this->expAst->generate_code(symbol_table,true);
		string temp_type = this->expAst->type->name;
		SymTable_local* temp_lst = gst->symlist.find(temp_type)->second->symtable;
		int temp_offset = temp_lst->symlist.find(identifier->identifier)->second->offset;
		cout<<"#Member"<<endl;
		stack_pop();
		cout<<"addi $s1 $s1 "<<temp_offset<<endl;
		if(!left)
			cout<<"lw $s1 ($s1)"<<endl;
		stack_push();
	}


	Arrow::Arrow(){
	}

	Arrow::Arrow(ExpAst* expAst , Identifier* identifier){
		this->expAst=expAst;
		this->identifier=identifier;
	}
	void Arrow::print(){
		cout<<"(Arrow ";
		expAst->print();
		cout<<" ";
		identifier->print();
		cout<<")";
	}
	void Arrow::generate_code(SymTable_local* symbol_table, int left){
		this->expAst->generate_code(symbol_table); // since baad me derefernce karne hi wala hai
		string temp_type = this->expAst->type->Dereference()->name;
		SymTable_local* temp_lst = gst->symlist.find(temp_type)->second->symtable;
		int temp_offset = temp_lst->symlist.find(identifier->identifier)->second->offset;
		stack_pop();
		cout<<"#Arrow"<<endl;
		// cout<<"lw $s1 ($s1)"<<endl;
		cout<<"addi $s1 $s1 "<<temp_offset<<endl;
		if(!left)
			cout<<"lw $s1 ($s1)"<<endl;
		stack_push();
	}

int x = 0;
string newlabel(){
	x=x+1;
	return "L"+to_string(x-1)+"";
}

//No use-------------
	Pointer::Pointer(){
		
	}

	Pointer::Pointer(ExpAst* expAst){
	    this->expAst=expAst;
	}

    void Pointer::print(){
        cout<<"(Pointer ";
        expAst->print();    
        cout<<")";
    }
	void Pointer::generate_code(SymTable_local* symbol_table, int left){
		// this->expAst->generate_code(symbol_table,)
	}
    
    Deref::Deref(){
    	
    }
    
	Deref::Deref(ExpAst* expAst){
	    this->expAst=expAst;
	}
	
	void Deref::print(){
        cout<<"(Deref ";
	    expAst->print();
	    cout<<")";
	}
	void Deref::generate_code(SymTable_local* symbol_table, int left){
		if (left)
			this->expAst->generate_code(symbol_table);
		else{
			this->expAst->generate_code(symbol_table);
			stack_pop();
			cout<<"lw $s1 ($s1)"<<endl;
			stack_push();
		}

	}
//--------------------------------------------No use
