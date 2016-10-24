all:    Scanner.ih Scanner.h Scannerbase.h lex.cc parse.cc main.cc Parser.h Parserbase.h Parser.ih ast.cpp symtable.h symtable.cpp
	g++   --std=c++0x lex.cc parse.cc main.cc ast.cpp symtable.cpp -o parser
	./parser < test.c 
        
Scanner.ih: lex.l
	if [ ! -e Scanner.ih ]; then \
		flexc++ lex.l;\
                sed -i '/include/a #include "Parserbase.h"' Scanner.ih; \
	fi

parse.cc Parser.h Parserbase.h Parser.ih: parse.y
	bisonc++  --construction parse.y; 
	sed -i '/insert preincludes/a #include "ast.h"' Parserbase.h ;
	sed -i '/insert preincludes/a #include "symtable.h"' Parserbase.h ;
	sed -i '/insert class.i/a int linenum=1;' parse.cc;
	sed -i '/insert preincludes/a extern int linenum;' Parserbase.h;
	sed -i '\|STYPE__     d_val__;| d' Parserbase.h;
	sed -i '\|typedef Meta__::SType STYPE__;| a \\tpublic: static STYPE__  d_val__; ' Parserbase.h;
	sed -i '\|void Parser::print__()| i Meta__::SType ParserBase::d_val__; ' parse.cc



.PHONY: clean     
clean:
	$(RM) Parser.ih Parser.h Parserbase.h parse.cc Scanner.ih Scanner.h Scannerbase.h lex.cc a.out graph.ps junk parse.y.output parser *~

