INT_LITERAL_RE 					    	 (0[xX][0-9a-fA-F]*)|([1-9][0-9]*)|(0[bB][01]*|0[0-7]*)
FLOAT_LITERAL_RE 						([0-9]+\.(([0-9]*)([eE]?)((\+|\-)?)[0-9]*))|([0-9]*\.(([0-9]+)([eE]?)(\+|\-)?[0-9]*))		
%%
[ \t] 													//skip
\/\/.*													//skip
"\n"                                                    {linenum++;}   
"int" 													{Parser::d_val__ = std::string (matched()); return Parser::INT;}
"void"													{Parser::d_val__ = std::string (matched()); return Parser::VOID;}
"float"													{Parser::d_val__ = std::string (matched()); return Parser::FLOAT;}
"struct"												{Parser::d_val__ = std::string (matched()); return Parser::STRUCT;}
{INT_LITERAL_RE}										{Parser::d_val__ = std::string (matched()); return Parser::INT_CONSTANT;}
{FLOAT_LITERAL_RE}										{Parser::d_val__ = std::string (matched()); return Parser::FLOAT_CONSTANT;}
\"[^"\\]*(\\[nt"\\][^"\\]*)*\"							{Parser::d_val__ = std::string (matched()); return Parser::STRING_LITERAL;}
"if"													{Parser::d_val__ = std::string (matched()); return Parser::IF;}
"else"													{Parser::d_val__ = std::string (matched()); return Parser::ELSE;}
"while"													{Parser::d_val__ = std::string (matched()); return Parser::WHILE;}
"for"													{Parser::d_val__ = std::string (matched()); return Parser::FOR;}
"return"												{Parser::d_val__ = std::string (matched()); return Parser::RETURN;}
([[:alpha:]]|_)([[:alnum:]]|_)*                     	{Parser::d_val__ = std::string (matched()); return Parser::IDENTIFIER;}
"||"													{Parser::d_val__ = std::string (matched()); return Parser::OR_OP;}
"&&"													{Parser::d_val__ = std::string (matched()); return Parser::AND_OP;}
"=="													{Parser::d_val__ = std::string (matched()); return Parser::EQ_OP;}
"!="													{Parser::d_val__ = std::string (matched()); return Parser::NE_OP;}
">="													{Parser::d_val__ = std::string (matched()); return Parser::GE_OP;}
"<="													{Parser::d_val__ = std::string (matched()); return Parser::LE_OP;}
"++"													{Parser::d_val__ = std::string (matched()); return Parser::INC_OP;}
"->"													{Parser::d_val__ = std::string (matched()); return Parser::PTR_OP;}
"{"														{Parser::d_val__ = std::string (matched()); return '{';}
"}"														{Parser::d_val__ = std::string (matched()); return '}';}
"["														{Parser::d_val__ = std::string (matched()); return '[';}
"]"														{Parser::d_val__ = std::string (matched()); return ']';}
"("														{Parser::d_val__ = std::string (matched()); return '(';}
")"														{Parser::d_val__ = std::string (matched()); return ')';}
"="														{Parser::d_val__ = std::string (matched()); return '=';}
">"														{Parser::d_val__ = std::string (matched()); return '>';}
"<"														{Parser::d_val__ = std::string (matched()); return '<';}
"+"														{Parser::d_val__ = std::string (matched()); return '+';}
"-"														{Parser::d_val__ = std::string (matched()); return '-';}
"*"														{Parser::d_val__ = std::string (matched()); return '*';}
"&"														{Parser::d_val__ = std::string (matched()); return '&';}
"/"														{Parser::d_val__ = std::string (matched()); return '/';}
"!"														{Parser::d_val__ = std::string (matched()); return '!';}
","														{Parser::d_val__ = std::string (matched()); return ','; }
"\;"													{Parser::d_val__ = std::string (matched()); return ';'; }
'.'	    											    {Parser::d_val__ = std::string (matched()); return '.'; }
.														{Parser::d_val__ = std::string (matched()); return Parser::OTHERS; }
