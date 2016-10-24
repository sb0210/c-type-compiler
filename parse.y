// %debug
%scanner Scanner.h
%scanner-token-function d_scanner.lex()
%token VOID INT FLOAT FLOAT_CONSTANT INT_CONSTANT AND_OP OR_OP EQ_OP NE_OP GE_OP LE_OP STRING_LITERAL IF ELSE WHILE FOR RETURN STRUCT IDENTIFIER INC_OP PTR_OP OTHERS
%polymorphic expAst : ExpAst* ; stmtAst : StmtAst*; Int : int; Float : float; String : string; OPUnaryNames : OPUnaryNames;exp_list : std::vector<ExpAst*>* ; seq: Seq*

%type <expAst> expression logical_and_expression logical_or_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression postfix_expression primary_expression constant_expression 
%type <stmtAst> selection_statement iteration_statement assignment_statement function_definition compound_statement statement
%type <OPUnaryNames> unary_operator
%type <String> STRING_LITERAL IDENTIFIER INT_CONSTANT FLOAT_CONSTANT
%type <exp_list> expression_list
%type <seq> statement_list  translation_unit

%%


S :
    translation_unit {
        if(debugger)    
           gst->print();
        cout<<".data"<<endl;
        cout<<"SPACE"<<": .asciiz "<<"\" \""<<endl;
        cout<<"ENDLINE"<<": .asciiz "<<"\"\\n\""<<endl;
        for(int i=0;i<global_strings.size();i++){
            cout<<global_strings[i].label<<": .asciiz "<<global_strings[i].str<<endl;
        }    
        cout<<".text"<<endl;
        for(int i=0;i<$1->stmt_list.size();i++){
            cout<<$1->stmt_list[i]->function_record->symbol_name<<":"<<endl;//function label
            cout<<"#store return add"<<endl;
            cout<<"addi $sp $sp -4"<<endl;
            cout<<"sw $ra ($sp)"<<endl<<endl;//return address

            cout<<"#dynamic link "<<endl;
            cout<<"addi $sp $sp -4"<<endl;//
            cout<<"sw $fp ($sp)"<<endl;
            cout<<"addi $fp $sp 0"<<endl<<endl;//

            cout<<"#space for locals"<<endl;
            cout<<"addi $sp $sp "<<$1->stmt_list[i]->function_record->symtable->locals_size<<endl<<endl;//size for locals
            $1->stmt_list[i]->generate_code($1->stmt_list[i]->function_record->symtable);
            
            cout<<"#functio body done"<<endl;

            cout<<"#restore fp"<<endl;
            cout<<"addi $sp $fp 0"<<endl;
            cout<<"lw $fp ($fp)"<<endl;
            cout<<"addi $sp $sp 4"<<endl<<endl;

            cout<<"lw $ra ($sp)"<<endl;
            cout<<"addi $sp $sp 4"<<endl;
            
            cout<<"#skip params"<<endl;
            cout<<"addi $sp $sp "<<$1->stmt_list[i]->function_record->symtable->params_size()<<endl;
            cout<<"jr $ra"<<endl;
        }

            
    };

translation_unit 
        :  struct_specifier {
            $$ = new Seq();
        }
 		| function_definition
        {
            if(debugger){
                $1->print();
                cout<<endl;
            }
            $$ = new Seq();
            $$->stmt_list.push_back($1);
        }
 		| translation_unit function_definition {
            if(debugger){
                $2->print();
                cout<<endl;
            }
            $$ = $1;
            $$->stmt_list.push_back($2);
        }
        | translation_unit struct_specifier{
            $$=$1;
        }
        ;

struct_specifier 
        : STRUCT IDENTIFIER '{' {
        	lst = new SymTable_local();
        	is_param=0;
        	offset=0;
        	type = new BaseType($2);
    	    if(type->equal(Type_void)){
                 cerr<<linenum<<": error: variable or field "<<$2<<" declared of void type"<<endl;
                exit(0);
    	    }
    	
            if(gst->symlist.find($2)!=gst->symlist.end()){
                cerr<<linenum<<": error: redefinition of 'struct "<<$2<<"'"<<endl;
                exit(0);
            }
            else{
            	grc = new Record_global($2,sf_enum::STRUCT,type,0,lst);
            	gst->add($2,grc);
            }
        }
         declaration_list '}' ';' {
            width=-1*offset;
            gst->symlist.find($2)->second->size=width;
        	type = old_type;
        	width = old_width;
             lst->populate_offset(-1*offset );
            offset = 0;
        }
        ;

function_definition
	: type_specifier{
        	lst = new SymTable_local();
	} fun_declarator {
	        fun_name = symbol_name;
            if(gst->symlist.find(symbol_name)!=gst->symlist.end()){
                cerr<<linenum<<": error: redefinition of '"<<symbol_name<<"'"<<endl;
                delete lst;
                exit(0);
            }
            else{
            	grc = new Record_global(symbol_name,sf_enum::FUN,current_return_type,return_size,lst);
            	gst->add(symbol_name,grc); 
                grc_temp = grc;       
            }
        	type = old_type;
        	width = old_width;
        	is_param = false;
        }
	compound_statement {
        $$ = $5;
        $$->function_record=grc_temp;
        offset = 0;
    } 
	;

type_specifier                   // This is the information // that gets associated with each identifier
        : VOID { old_type = new BaseType("void"); old_width = 1; type=old_type;width=old_width;} 
        | INT  { old_type = new BaseType("int"); old_width = 4; type=old_type;width=old_width;} 
	| FLOAT { old_type = new BaseType("float"); old_width = 4; type=old_type;width=old_width;}
        | STRUCT IDENTIFIER {
            if(gst->symlist.find($2)==gst->symlist.end()){
                cerr<<linenum<<": error: storage size of'"<<$2<<"' isn't known"<<endl;
                old_width = 0;
                exit(0);
            }
            else if(gst->symlist.find($2)->second->sf==sf_enum::FUN){
                cerr<<linenum<<": error: storage size of'"<<$2<<"' isn't known"<<endl;
                old_width = 0;
                exit(0);
            }
            else{
                old_width=gst->symlist.find($2)->second->size;  
            }
            old_type = new BaseType($2);type=old_type;width=old_width;
        } // width ??
        ;

fun_declarator
	: IDENTIFIER '('{is_param = true; current_return_type=type;return_size=width;} parameter_list ')' { lst->populate_offset(-1*offset + size_ebp +4);symbol_name = $1; offset = 0;}
	| IDENTIFIER '(' ')' { current_return_type=type;return_size=width; symbol_name = $1;}
    | '*' fun_declarator { //The * is associated with the 
		type = new PointerType(type);
		width=4;
		current_return_type = type;
		return_size = width;
	}
	;                      //function name


parameter_list
	: parameter_declaration 
	| parameter_list ',' parameter_declaration 
	;

parameter_declaration
	: type_specifier declarator
    {
        offset=offset-width;
        if(lst->symlist.find(symbol_name)!=lst->symlist.end()){
            cerr<<linenum<<": error: redefinition of parameter '"<<$2<<"' isn't known"<<endl;
            exit(0);
        }
        if(type->equal(Type_void)){
                 cerr<<linenum<<": error: variable or field "<<symbol_name<<" declared of void type"<<endl;
                exit(0);
    	}
        else{
            lrc = new Record_local(symbol_name, is_param, type, width, offset); 
            lst->add(symbol_name,lrc);
        }
            type = old_type;
            width = old_width;
    } 
        ;
	

declarator
	: IDENTIFIER {
            symbol_name=$1;        
	}
	| declarator '[' primary_expression']' {// check separately that it is a constant 
        if(!is_intconst){
            cerr<<linenum<<": error: size of array '"<<symbol_name<<"' has non-constant-integer type"<<endl;
            exit(0);
        }
        Type* t = type;
        if(type->type==0){
            if(is_param==false)  
                type = new ArrayType(val,type);
            else
                type = new PointerType(type);
        }
        else{
        // type->type= new Array(val,type);
            while(t->type->type!=0){
                t=t->type;
            }
            
            t->type= new ArrayType(val,t->type);
        }
        if(is_param)
            width=4;
        else    
        	width = val * width; //dimension
    }
    | '*' {width=4;} declarator {
        Type* t = type;
        if(type->type==0){
            type = new PointerType(type);
            width=4;
        }
        else{
        // type->type= new Array(val,type);
            while(t->type->type!=0){
                t=t->type;
            }
            
            t->type= new PointerType(t->type);
        }
    }
    ;
declaration_list
        : declaration  					
        | declaration_list declaration
	;

declaration
	: type_specifier declarator_list';' 
	;

declarator_list
	: declarator {
        offset=offset-width;
         if(type->equal(grc->type) && grc->sf==sf_enum::STRUCT){
            cerr<<linenum<<" error field '"<<symbol_name<<" , not completely declared"<<endl;
            exit(0);
        }
        else if(lst->symlist.find(symbol_name)!=lst->symlist.end()){
            cerr<<linenum<<": error: '"<<symbol_name<<"' redeclared as different kind of symbol"<<endl;
            exit(0);
        }
        if(type->equal(Type_void)){
                 cerr<<linenum<<": error: variable or field "<<symbol_name<<" declared of void type"<<endl;
                exit(0);
    	    }
        else{
            lrc = new Record_local(symbol_name, is_param, type, width, offset); 
            lst->add(symbol_name,lrc);
        }
        type = old_type;
        width = old_width;
    }
	| declarator_list ',' declarator {
        offset=offset-width;
        if(lst->symlist.find(symbol_name)!=lst->symlist.end()){
            if(lst->symlist.find(symbol_name)->second->type->equal(type))
                cerr<<linenum<<": error: redeclaration of '"<<symbol_name<<"' with no linkage"<<endl;
            else
                cerr<<linenum<<": error: conflicting types for '"<<symbol_name<<"'"<<endl;               
            exit(0);    
        }
        else{
            lrc = new Record_local(symbol_name, is_param, type, width, offset); 
            lst->add(symbol_name,lrc);
        }
        type = old_type;
        width = old_width;
    }
 	;



primary_expression              // The smallest expressions, need not have a l_value
        : IDENTIFIER { // primary expression has IDENTIFIER now
            is_intconst=false;
            $$ = new Identifier($1);
            if(lst->symlist.find($1)!=lst->symlist.end()){
                $$->setType(lst->symlist.find($1)->second->type);
                $$->l_value=1;
                if($$->type->pab=="array"){
                    $$->l_value=false;
                }
            }
            else{
                cerr<<linenum<<": error: '"<<$1<<"' undeclared"<<endl; 
                exit(0);               
            }
        }           
        | INT_CONSTANT {
            is_intconst=true;
            string val_string = $1;
            val = atoi(val_string.c_str());
            $$ = new IntConst($1);
            $$->setType(new BaseType("int"));
            $$->l_value=0;
        }
        | FLOAT_CONSTANT{
            is_intconst =false;
            $$ = new FloatConst($1);
            $$->setType(new BaseType("float"));
            $$->l_value=0;
        }
        | STRING_LITERAL {
            is_intconst = false;
            StringObject s;
            s.str = $1;
            s.label = newlabel();
            global_strings.push_back(s);
            $$ = new StringConst(s.label);            
            $$->setType(new BaseType("string"));
            $$->l_value=0;
        }
        | '(' expression ')' {
            is_intconst = false;
            $$=$2;
        }
        ;

compound_statement
    : '{' '}' {
        $$ = new Empty();/*not sure check */
    }
    | '{' statement_list '}'{
        $$ = $2;
    } 
    | '{' declaration_list statement_list '}' {
        $$=$3;
    } 
    ;

statement_list
    : statement{
        $$ = new Seq();
        $$->stmt_list.push_back($1);
    }   
    | statement_list statement{
        $$ = $1;
        $$->stmt_list.push_back($2);

    }   
    ;

statement
    : '{' statement_list '}'  //a solution to the local decl problem
    {
        $$ = $2;
    }
    | selection_statement {
        $$ = $1;
    }   
    | iteration_statement {
        $$ = $1;
    }   
    | assignment_statement{
        $$ = $1;
    }   
    | RETURN expression ';' {
        
        if(current_return_type->equal(Type_voidstar) && $2->type->pab=="pointer" ){
            // $$ = new Return($2);
            // $$->type=current_return_type;
        }
        else if (current_return_type->pab=="pointer" && $2->type->equal(Type_voidstar)){
            // $$ = new Return($2);
            // $$->type=$1->type;
        }
        else if(!current_return_type->equal($2->type)){
            cerr<<linenum<<": error: Return Type does not match function signature"<<endl; 
            exit(0);
        }
        $$ = new Return($2);
    }
    ;
assignment_statement
	: ';' {
        $$ = new Empty();
    } 								
	|  expression ';' {
        $$ = $1;
    }  
	;

expression                                   //assignment expressions are right associative
        :  logical_or_expression{
            $$=$1;
        }
        |
            unary_expression '=' expression {
                ExpAst* temp = $1;
                if($1->l_value==false){
                   cerr<<linenum<<": error: lvalue required as left operand of assignment"<<endl; 
                   exit(0);
                }
                else if($1->type->equal(Type_voidstar) && $3->type->pab=="pointer" ){
                    $$ = new Assign($1, $3);
                    $$->type=$1->type;
                }
                else if ($1->type->pab=="pointer" && $3->type->equal(Type_voidstar)){
                    $$ = new Assign($1, $3);
                    $$->type=$1->type;
                }
                else if(!$1->type->equal($3->type)){
                   cerr<<linenum<<": error: incompatible type while assigning to typr ";
                   $1->type->print();
                   cerr<<" from type ";
                   $3->type->print();
                   cerr<<endl; 
                   exit(0);
                }
                else {
                    if($1->type->equal(Type_int) && $1->type->name == "int" && $3->type->name == "float"){
                        $$ = new Assign($1, new ToInt($3));
                    }
                    else if ($1->type->equal(Type_int) && $1->type->name == "float" && $3->type->name == "int") {
                        $$ = new Assign($1, new ToFloat($3));
                    }
                    else {
                        $$ = new Assign($1, $3);
                    }
                    $$->type=temp->type;
                }
                $$->l_value=false;
            }
            
          // l_expression has been replaced by unary_expression.
        ;                                    // This may generate programs that are syntactically incorrect.
                                             // Eliminate them during semantic analysis.

logical_or_expression            // The usual hierarchy that starts here...
	: logical_and_expression {
        $$=$1;
    }
    | logical_or_expression OR_OP logical_and_expression{
        if($1->type->pab=="base" && !$1->type->equal(Type_int) ){
                cerr<<linenum<<": error: Invalid operand to binary operator ||"<<endl; 
                exit(0);
            }
            else if($3->type->pab=="base" && !$3->type->equal(Type_int) ){
                cerr<<linenum<<": error: Invalid operand to binary operator ||"<<endl; 
                exit(0);
            }
            else {
                $$ = new Op_binary(OPBinaryNames::OR, $1, $3 );
                $$->type= Type_int;
                $$->l_value=0;
            }
    }
	;

logical_and_expression
        : equality_expression{
            $$=$1;
        }
        | logical_and_expression AND_OP equality_expression {//???
            if($1->type->pab=="base" && !$1->type->equal(Type_int) ){
                cerr<<linenum<<": error: Invalid operand to binary operator &&"<<endl; 
                exit(0);
            }
            else if($3->type->pab=="base" && !$3->type->equal(Type_int) ){
                cerr<<linenum<<": error: Invalid operand to binary operator &&"<<endl; 
                exit(0);
            }
            else {
                $$ = new Op_binary(OPBinaryNames::AND, $1, $3 );
                $$->type= Type_int;
                $$->l_value=0;
            }
        }
    ;

equality_expression
    : relational_expression {
        $$=$1;
    }
    | equality_expression EQ_OP relational_expression{
        if(!$1->type->equal(Type_int) || !$3->type->equal(Type_int)){
        if($1->type->equal($3->type))
            {
                if($1->type->pab=="base" && $1->type->name!="string"){
                    cerr<<linenum<<": error comparison between incompatible types"<<endl;
                    exit(0);
                }
                else{
                    $$ = new Op_binary(OPBinaryNames::EQ_OP, $1, $3 );
                    $$->type = Type_int;
                    $$->l_value=false;
                }
            }
            
            else {
                cerr<<linenum<<": error comparison between incompatible types"<<endl;
                exit(0);
            }
        }
        else if($1->type->name=="int" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::EQ_OP, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="int" && $3->type->name=="float"){
            //*float TODO
            $$ = new Op_binary(OPBinaryNames::EQ_OP,new ToFloat($1), $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::EQ_OP, $1,new ToFloat($3));
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="float"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::EQ_OP, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
    }   
    | equality_expression NE_OP relational_expression{
                if(!$1->type->equal(Type_int) || !$3->type->equal(Type_int)){

        if($1->type->equal($3->type))
            {
                if($1->type->pab=="base" && $1->type->name!="string"){
                    cerr<<linenum<<": error comparison between incompatible types"<<endl;
                    exit(0);
                }
                else{
                    $$ = new Op_binary(OPBinaryNames::NE_OP, $1, $3 );
                    $$->type = Type_int;
                    $$->l_value=false;
                }
            }
            
            else {
                cerr<<linenum<<": error comparison between incompatible types"<<endl;
                exit(0);
            }
        }
        else if($1->type->name=="int" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::NE_OP, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="int" && $3->type->name=="float"){
            //*float TODO
            $$ = new Op_binary(OPBinaryNames::NE_OP,new ToFloat($1), $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::NE_OP, $1,new ToFloat($3));
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="float"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::NE_OP, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
    }
    ;
relational_expression
    : additive_expression{
        $$=$1;
    }
    | relational_expression '<' additive_expression{
        
        if(!$1->type->equal(Type_int) || !$3->type->equal(Type_int)){
            if($1->type->equal($3->type))
            {
                if($1->type->pab=="base" && $1->type->name!="string"){
                    cerr<<linenum<<": error comparison between incompatible types"<<endl;
                    exit(0);
                }
                else{
                    $$ = new Op_binary(OPBinaryNames::LT_INT, $1, $3 );
                    $$->type = Type_int;
                    $$->l_value=false;
                }
            }
            
            else {
                cerr<<linenum<<": error comparison between incompatible types"<<endl;
                exit(0);
            }
        }
        else if($1->type->name=="int" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::LT_INT, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="int" && $3->type->name=="float"){
            //*float TODO
            $$ = new Op_binary(OPBinaryNames::LT_FLOAT,new ToFloat($1), $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::LT_FLOAT, $1,new ToFloat($3));
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="float"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::LT_FLOAT, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
    }
    | relational_expression '>' additive_expression{
        if(!$1->type->equal(Type_int) || !$3->type->equal(Type_int)){
            if($1->type->equal($3->type)){
                $$ = new Op_binary(OPBinaryNames::GT_INT, $1, $3 );
                $$->type = Type_int;
                $$->l_value=false;
            }
            else {
                cerr<<linenum<<": error comparison between incompatible types"<<endl;
                exit(0);
            }
        }
        else if($1->type->name=="int" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::GT_INT, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="int" && $3->type->name=="float"){
            //*float TODO
            $$ = new Op_binary(OPBinaryNames::GT_FLOAT,new ToFloat($1), $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::GT_FLOAT, $1,new ToFloat($3));
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="float"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::GT_FLOAT, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
    }
    | relational_expression LE_OP additive_expression {
        if(!$1->type->equal(Type_int) || !$3->type->equal(Type_int)){
            if($1->type->equal($3->type)){
                $$ = new Op_binary(OPBinaryNames::LE_OP_INT, $1, $3 );
                $$->type = Type_int;
                $$->l_value=false;
            }
            else {
                cerr<<linenum<<": error comparison between incompatible types"<<endl;
                exit(0);
            }
        }
        else if($1->type->name=="int" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::LE_OP_INT, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="int" && $3->type->name=="float"){
            //*float TODO
            $$ = new Op_binary(OPBinaryNames::LE_OP_FLOAT,new ToFloat($1), $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::LE_OP_FLOAT, $1,new ToFloat($3));
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="float"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::LE_OP_FLOAT, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
    }
    | relational_expression GE_OP additive_expression{
        if(!$1->type->equal(Type_int) || !$3->type->equal(Type_int)){
            if($1->type->equal($3->type)){
                $$ = new Op_binary(OPBinaryNames::GE_OP, $1, $3 );
                $$->type = Type_int;
                $$->l_value=false;
            }
            else {
                cerr<<linenum<<": error comparison between incompatible types"<<endl;
                exit(0);
            }
        }
        else if($1->type->name=="int" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::GE_OP_INT, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="int" && $3->type->name=="float"){
            //*float TODO
            $$ = new Op_binary(OPBinaryNames::GE_OP_FLOAT,new ToFloat($1), $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::GE_OP_FLOAT, $1,new ToFloat($3));
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="float"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::GE_OP_FLOAT, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
    }
    ;

additive_expression 
    : multiplicative_expression{
        $$=$1;
    }
    | additive_expression '+' multiplicative_expression {
        if(!$1->type->equal(Type_int) || !$3->type->equal(Type_int)){
            cerr<<linenum<<": error: +  operator not defined for this type"<<endl;
            exit(0);
        }
        else if($1->type->name=="int" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::PLUS_INT, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="int" && $3->type->name=="float"){
            //*float TODO
            $$ = new Op_binary(OPBinaryNames::PLUS_FLOAT,new ToFloat($1), $3 );
            $$->type = Type_float;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::PLUS_FLOAT, $1,new ToFloat($3));
            $$->type = Type_float;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="float"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::PLUS_FLOAT, $1, $3 );
            $$->type = Type_float;
            $$->l_value=false;

        }
    }
    | additive_expression '-' multiplicative_expression {
        if(!$1->type->equal(Type_int) || !$3->type->equal(Type_int)){
            cerr<<linenum<<": error: - operator not defined for this type"<<endl;               
            exit(0);
        }
        else if($1->type->name=="int" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::MINUS_INT, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="int" && $3->type->name=="float"){
            //*float TODO
            $$ = new Op_binary(OPBinaryNames::MINUS_FLOAT,new ToFloat($1), $3 );
            $$->type = Type_float;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::MINUS_FLOAT, $1,new ToFloat($3));
            $$->type = Type_float;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="float"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::MINUS_FLOAT, $1, $3 );
            $$->type = Type_float;
            $$->l_value=false;

        }
    }
    ;

multiplicative_expression
    : unary_expression{
        $$ = $1;
    }
    | multiplicative_expression '*' unary_expression {
        // $1->print();cout<<endl;
        // $3->print();cout<<endl;
        if(!$1->type->equal(Type_int) || !$3->type->equal(Type_int)){
            cerr<<linenum<<": error: *operator not defined for this type"<<endl;               
            exit(0);
        }
        else if($1->type->name=="int" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::MULT_INT, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="int" && $3->type->name=="float"){
            //*float TODO
            $$ = new Op_binary(OPBinaryNames::MULT_FLOAT, new ToFloat($1), $3 );
            $$->type = Type_float;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::MULT_FLOAT, $1,new ToFloat($3));
            $$->type = Type_float;
            $$->l_value=false;

        }else if($1->type->name=="float" && $3->type->name=="float"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::MULT_FLOAT, $1, $3 );
            $$->type = Type_float;
            $$->l_value=false;

        }
        // $$->print();
        cout<<endl;
    }
    | multiplicative_expression '/' unary_expression {
        if(!$1->type->equal(Type_int) || !$3->type->equal(Type_int)){
            cerr<<linenum<<": error: / operator not defined for this type"<<endl;               
            exit(0);
        }
        else if($1->type->name=="int" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::DIV_INT, $1, $3 );
            $$->type = Type_int;
            $$->l_value=false;

        }
        else if($1->type->name=="int" && $3->type->name=="float"){
            //*float TODO
            $$ = new Op_binary(OPBinaryNames::DIV_FLOAT,new ToFloat($1), $3 );
            $$->type = Type_float;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="int"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::DIV_FLOAT, $1,new ToFloat($3));
            $$->type = Type_float;
            $$->l_value=false;

        }
        else if($1->type->name=="float" && $3->type->name=="float"){
            //*int TODO
            $$ = new Op_binary(OPBinaryNames::DIV_FLOAT, $1, $3 );
            $$->type = Type_float;
            $$->l_value=false;

        }
    }
    ;
unary_expression
	: postfix_expression  {
        $$ = $1;
    }			
	| unary_operator unary_expression {
       $$ = new Op_unary($1 , $2);
       if($1 == OPUnaryNames::STAR){
           
           if($2->type->Dereference()==NULL){
            cerr<<linenum<<": error: illegal operand for unary operator *"<<endl;  
            exit(0);
           }
           else{
                $$->setType($2->type->Dereference());
                $$->l_value=true;
           }
       }
       else if($1==OPUnaryNames::ADDRESS){
           if($2->l_value==true){
               $$->setType(new PointerType($2->type));
               $$->l_value=false;
           }
           else{
               cerr<<linenum<<": error: lvalue required as unary '&' operand"<<endl;
               exit(0);
           }
       }
       else if ($1==OPUnaryNames::UMINUS){
           $$->l_value=false;
           if($2->type->equal(Type_int)){
           $$->setType($2->type);
           $$->l_value=false;
           }else{
               cerr<<linenum<<": error: Minus Operator not applicable"<<endl;
               exit(0);
           }
       }
       else{ //NOT
            if($2->type->pab == "base" && !$2->type->equal(Type_int)){
                cerr<<linenum<<": error: Not operator not applicable"<<endl;
                exit(0);
            }
            else{
                $$->l_value=false;
                $$->setType(Type_int);
           }
       }
    }    // unary_operator can only be '*' on the LHS of '='
	;                                     // you have to enforce this during semantic analysis

postfix_expression
	: primary_expression{
        $$=$1;
    }  				
    | IDENTIFIER '(' ')' {
        $$ = new Funcall(new Identifier($1));
        grc = is_global($1,sf_enum::FUN);
        vector<Record_local*> v = grc->params();
        if(grc!=NULL){
            if(v.size()==0){
                $$->setType(grc->type);
                $$->l_value = false;
            }
            else{
                cerr<<linenum<<": error: Parameters mismatch. Extra params error"<<endl;
                exit(0);
            }
        }
        else{
            cerr<<linenum<<": error: Function not defined"<<endl;
            exit(0);
        }
    }				    // Cannot appear on the LHS of '='. Enforce this.
	    | IDENTIFIER '(' expression_list ')'{
            grc = is_global($1,sf_enum::FUN);
            
            if($1=="printf"){
                // cout<<"printf zindabad"<<endl;
                $$ = new Funcall(new Identifier($1), $3);
                $$->setType(Type_void);
                $$->l_value = false;
            }
            else{
                if(grc==NULL){
                    cerr<<linenum<<": error: Function not defined"<<endl;
                    exit(0);
                }
                else{
                    vector<Record_local*> v = grc->params();
                    if(v.size()<$3->size()){
                        cerr<<linenum<<": error: too many arguments to function ‘"<<$1<<"’"<<endl;
                        exit(0);
                    }
                    else if(v.size()>$3->size()){
                        cerr<<linenum<<": error: too few arguments to function ‘"<<$1<<"’"<<endl;
                        exit(0);
                    }
                    for(int i=0;i<v.size();i++){
                        if(v[i]->type->equal((*($3))[i]->type)){
                            if(v[i]->type->pab=="base"){
                                if(v[i]->type->name=="int" && (*($3))[i]->type->name=="float"){
                                    (*($3))[i] = new ToInt((*($3))[i]);
                                }
                                if(v[i]->type->name=="float" && (*($3))[i]->type->name=="int"){
                                    (*($3))[i] = new ToFloat((*($3))[i]);
                                }
                            }
                        }
                        else if(v[i]->type->equal(Type_voidstar) && (*($3))[i]->type->pab=="pointer" ){
                            // $$ = new Return($2);
                            // $$->type=current_return_type;
                        }
                        else if (v[i]->type->pab=="pointer" && (*($3))[i]->type->equal(Type_voidstar)){
                            // $$ = new Return($2);
                            // $$->type=$1->type;
                        }
                        else{
                            cerr<<linenum<<": error:  incompatible type for argument "<<i+1<<" of '"<<$1<<"’"<<endl;
                            exit(0);
                        }
                    }
                    // $$-params==$3;
                }
                $$ = new Funcall(new Identifier($1), $3);
                $$->setType(grc->type);
                    $$->l_value = false;
            }
                // $$->setType(current_return_type);

        }    // Cannot appear on the LHS of '='  Enforce this.
        | postfix_expression '[' expression ']' {
            if(!$3->type->equal(Type_int)){
                cerr<<linenum<<": error: array subscript is not an integer"<<endl;
                exit(0);
            }
            else if($3->type->name=="float"){
                cerr<<linenum<<": error: array subscript is not an integer"<<endl;
                exit(0);
            }
            if($1->type->pab=="base"){
                cerr<<linenum<<": cannot arayref a base type"<<endl;
                exit(0);
            }
            ExpAst* temp = $1;
            $$ = new ArrayRef($1, $3);
            $$->type = temp->type->Dereference();
            if($$->type->pab=="base"){
                $$->l_value=true;
            }
            else{
                $$->l_value=false;
            }
        }
        | postfix_expression '.' IDENTIFIER {

            ExpAst* temp = $1;
            
            if($1->type==0){
                cerr<<linenum<<": error: postfix_expression type not set"<<endl;
                exit(0);
            }
            if($1->type->pab=="base"){
                string sname = $1->type->name;
                grc = is_global(sname,sf_enum::STRUCT);
                if(grc == NULL){
                    cerr<<linenum<<": error: error struct not defined"<<endl;
                    exit(0);
                }
                else{
                    if(grc->symtable->symlist.find($3)==grc->symtable->symlist.end()){
                        cerr<<linenum<<": error: error no member "<< $3<< " in struct "<<sname<<endl;
                        exit(0);
                    }
                    else{
                        $$ = new Member(temp, new Identifier($3));
                        $$->setType(grc->symtable->symlist.find($3)->second->type);
                        $$->l_value=true;
                    }
                }
            }
            else{
                cerr<<linenum<<": error: request for member '"<<$3<<"' in something not a structure or union"<<endl;
                exit(0);
            }
        }
        | postfix_expression PTR_OP IDENTIFIER {
            if($1->type==0){
                cerr<<linenum<<": error: postfix_expression type not set"<<endl;
                exit(0);
            }
            Type * temp = $1->type->Dereference();
            if(temp==0){
                cerr<<linenum<<": error: Dereference not applicable"<<endl;
                exit(0);

            }
             if(temp->type==0){
                string sname = temp->name;
                grc = is_global(sname,sf_enum::STRUCT);
                if(grc == NULL){
                    cerr<<linenum<<": error: struct not defined"<<endl;
                    exit(0);
                }
                else{
                    if(grc->symtable->symlist.find($3)==grc->symtable->symlist.end()){
                       cerr<<linenum<<": error: no member "<< $3<< " in struct "<<sname<<endl;
                        exit(0);
                    }
                    else{
                        $$ = new Arrow($1, new Identifier($3));
                        $$->setType(grc->symtable->symlist.find($3)->second->type);
                        $$->l_value = true; //check
                    }
                }
            }

            
        }
	    | postfix_expression INC_OP {
	        if($1->l_value==false){
	            
	            cerr<<linenum<<": error: lvalue required as increment operand"<<endl;
                exit(0);
	        }
	        else{
	            ExpAst* temp = $1;
	            $$ = new Op_unary(OPUnaryNames::PP , $1);
	            $$->type = temp->type;
	            $$->l_value=false;
	        }
            
        }       // Cannot appear on the LHS of '='   Enforce this
	;

// There used to be a set of productions for l_expression at this point.

expression_list
        : expression {
            $$ = new std::vector<ExpAst*>;
            ($$)->push_back($1);
        }
        | expression_list ',' expression{
            $$ = $1;
            ($$)->push_back($3);
        }
    ;

unary_operator
        : '-'{
            $$ = OPUnaryNames::UMINUS;   
        }
	   | '!'{
            $$ = OPUnaryNames::NOT;
        }
        | '&'{
            $$ = OPUnaryNames::ADDRESS;
        }
        | '*' 	{
            $$ = OPUnaryNames::STAR;
        }
	;

selection_statement
        : IF '(' expression ')' statement ELSE statement {
            $$=new If($3, $5, $7);
        }
	;

iteration_statement
	: WHILE '(' expression ')' statement{
        $$ = new While($3,$5);
    } 	
	| FOR '(' expression ';' expression ';' expression ')' statement{
        $$ = new For($3,$5, $7,$9);

    }  //modified this production
        ;

