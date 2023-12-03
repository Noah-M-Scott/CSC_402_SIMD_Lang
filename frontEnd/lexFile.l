%option caseless
%option noyywrap



%%
[ \r\t]+	 	{ continue; }

[\n]			{ IN_COM = 0; GLOBAL_LINE_NUMBER++; continue; }

[0-9]+"."[0-9]+  	{ if(IN_COM) continue; yylval.FLOAT = strdup(yytext); return FLOAT; } //float
"-"[0-9]+"."[0-9]+  	{ if(IN_COM) continue; yylval.FLOAT = strdup(yytext); return FLOAT; } //float

[0-9]+	 		{ if(IN_COM) continue; yylval.DECIMAL = strdup(yytext); return DECIMAL; } //decimal
"-"[0-9]+	 	{ if(IN_COM) continue; yylval.DECIMAL = strdup(yytext); return DECIMAL; } //decimal

"0x"[0-9a-fA-F]+ 	{ if(IN_COM) continue; yylval.HEX = strdup(yytext); return HEX; } //hex
"0b"[0-1]+		{ if(IN_COM) continue; yylval.BINARY = strdup(yytext); return BINARY; } //binary


"//"		{ IN_COM = 1; continue; }

"/*"		{ IN_COM = 1; continue; }
"*/"		{ IN_COM = 0; continue; }


"+|"		{ if(IN_COM) continue; return ADD_BAR_OP; }
"-|"		{ if(IN_COM) continue; return SUB_BAR_OP; }
"<<"		{ if(IN_COM) continue; return LTLT_OP; }
">>"		{ if(IN_COM) continue; return GTGT_OP; }
"=="		{ if(IN_COM) continue; return EQUEQU_OP; }
">="		{ if(IN_COM) continue; return GTEQU_OP; }
"<="		{ if(IN_COM) continue; return LTEQU_OP; }
"!="		{ if(IN_COM) continue; return NOTEQU_OP; }
"&&"		{ if(IN_COM) continue; return ANDAND_OP; }
"||"		{ if(IN_COM) continue; return OROR_OP; }
"..."		{ if(IN_COM) continue; return DOTDOTDOT_OP; }
"EXTERN"	{ if(IN_COM) continue; return EXTERN_OP; }
"GLOBAL"	{ if(IN_COM) continue; return GLOBAL_OP; }
"VOID"		{ if(IN_COM) continue; return VOID_OP; }
"BYTE"		{ if(IN_COM) continue; return BYTE_OP; }
"WORD"		{ if(IN_COM) continue; return WORD_OP; }
"LONG"		{ if(IN_COM) continue; return LONG_OP; }
"INT"		{ if(IN_COM) continue; return LONG_OP; }
"QUAD"		{ if(IN_COM) continue; return QUAD_OP; }
"SINGLE"	{ if(IN_COM) continue; return SINGLE_OP; }
"DOUBLE"	{ if(IN_COM) continue; return DOUBLE_OP; }
"CONST"		{ if(IN_COM) continue; return CONST_OP; }
"STORED"	{ if(IN_COM) continue; return SHARED_OP; }
"STRD"		{ if(IN_COM) continue; return SHARED_OP; }
"IF"		{ if(IN_COM) continue; return IF_OP; }
"ELSE"		{ if(IN_COM) continue; return ELSE_OP; }
"WHILE"		{ if(IN_COM) continue; return WHILE_OP; }
"DO"		{ if(IN_COM) continue; return DO_OP; }
"FOR"		{ if(IN_COM) continue; return FOR_OP; }
"BREAK"		{ if(IN_COM) continue; return BREAK_OP; }
"CONTINUE"	{ if(IN_COM) continue; return CONTINUE_OP; }
"SWITCH"	{ if(IN_COM) continue; return SWITCH_OP; }
"CASE"		{ if(IN_COM) continue; return CASE_OP; }
"DEFAULT"	{ if(IN_COM) continue; return DEFAULT_OP; }
"RETURN"	{ if(IN_COM) continue; return RETURN_OP; }
"GOTO"		{ if(IN_COM) continue; return GOTO_OP; }
"INCLUDE"	{ if(IN_COM) continue; return INCLUDE_OP; }
"IFDEF"		{ if(IN_COM) continue; return IFDEF_OP; }
"IFNDEF"	{ if(IN_COM) continue; return IFNDEF_OP; }
"THEN"		{ if(IN_COM) continue; return THEN_OP; }
"DEFINE"	{ if(IN_COM) continue; return DEFINE_OP; }
"UNDEF"		{ if(IN_COM) continue; return UNDEF_OP; }
"NOTHING"	{ if(IN_COM) continue; return NOTHING_OP; }
"BLOCK"		{ if(IN_COM) continue; return BLOCK_OP; }



"\""[.]+"\""		{ if(IN_COM) continue; yylval.STRING_LIT = strdup(yytext); return STRING_LIT; }



%%

/*
[a-zA-Z][a-zA-Z0-9_]*	{ if(IN_COM) continue; 
			  switch(checkConst(yytext){ 
				case(0): yylval.IDENT = strdup(yytextdup); 
				return IDENT; 
				
				case(1): yylval.DECIMAL = strdup(yytextdup); 
				return DECIMAL; 

				case(2): yylval.FLOAT = strdup(yytextdup); 
				return FLOAT; 
			  }
			}

[a-zA-Z_][a-zA-Z0-9_]+	{ if(IN_COM) continue; 
			  switch(checkConst(yytext){ 
				case(0): yylval.IDENT = strdup(yytextdup); 
				return IDENT; 
				
				case(1): yylval.DECIMAL = strdup(yytextdup); 
				return DECIMAL; 

				case(2): yylval.FLOAT = strdup(yytextdup); 
				return FLOAT; 
			  }
			}
*/

//int yywarp(void) {return 1;}











