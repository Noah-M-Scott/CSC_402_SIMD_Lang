%option caseless
%option noyywrap



%%
[ \r\t]+	 	{ continue; }

[\n]			{ GLOBAL_LINE_NUMBER++; continue; }

[0-9]+"."[0-9]+  	{ yylval.FLOAT = strdup(yytext); return FLOAT; } //float
"-"[0-9]+"."[0-9]+  	{ yylval.FLOAT = strdup(yytext); return FLOAT; } //float

[0-9]+	 		{ yylval.DECIMAL = strdup(yytext); return DECIMAL; } //decimal
"-"[0-9]+	 	{ yylval.DECIMAL = strdup(yytext); return DECIMAL; } //decimal

"0x"[0-9a-fA-F]+ 	{ yylval.HEX = strdup(yytext); return HEX; } //hex
"0b"[0-1]+		{ yylval.BINARY = strdup(yytext); return BINARY; } //binary


"+|"		{ return ADD_BAR_OP; }
"-|"		{ return SUB_BAR_OP; }
"<<"		{ return LTLT_OP; }
">>"		{ return GTGT_OP; }
"=="		{ return EQUEQU_OP; }
"|>"		{ return UGT_OP; }
"|<"		{ return ULT_OP; }
"|>="		{ return UGTEQU_OP; }
"|<="		{ return ULTEQU_OP; }
">="		{ return GTEQU_OP; }
"<="		{ return LTEQU_OP; }
"!="		{ return NOTEQU_OP; }
"&&"		{ return ANDAND_OP; }
"||"		{ return OROR_OP; }
"..."		{ return DOTDOTDOT_OP; }
"EXTERN"	{ return EXTERN_OP; }
"GLOBAL"	{ return GLOBAL_OP; }
"VOID"		{ return VOID_OP; }
"BYTE"		{ return BYTE_OP; }
"WORD"		{ return WORD_OP; }
"LONG"		{ return LONG_OP; }
"INT"		{ return LONG_OP; }
"QUAD"		{ return QUAD_OP; }
"SINGLE"	{ return SINGLE_OP; }
"DOUBLE"	{ return DOUBLE_OP; }
"CONST"		{ return CONST_OP; }
"SHARED"	{ return SHARED_OP; }
"IF"		{ return IF_OP; }
"ELSE"		{ return ELSE_OP; }
"WHILE"		{ return WHILE_OP; }
"DO"		{ return DO_OP; }
"FOR"		{ return FOR_OP; }
"BREAK"		{ return BREAK_OP; }
"CONTINUE"	{ return CONTINUE_OP; }
"SWITCH"	{ return SWITCH_OP; }
"CASE"		{ return CASE_OP; }
"DEFAULT"	{ return DEFAULT_OP; }
"RETURN"	{ return RETURN_OP; }
"GOTO"		{ return GOTO_OP; }
"INCLUDE"	{ return INCLUDE_OP; }
"IFDEF"		{ return IFDEF_OP; }
"IFNDEF"	{ return IFNDEF_OP; }
"THEN"		{ return THEN_OP; }
"DEFINE"	{ return DEFINE_OP; }
"UNDEF"		{ return UNDEF_OP; }
"NOTHING"	{ return NOTHING_OP; }


"\""[.]+"\""		{ yylval.STRING_LIT = strdup(yytext); return STRING_LIT; }

[a-zA-Z][a-zA-Z0-9_]*	{ yylval.IDENT = strdup(yytext); return IDENT; }
[a-zA-Z_][a-zA-Z0-9_]+	{ yylval.IDENT = strdup(yytext); return IDENT; }


%%


//int yywarp(void) {return 1;}











