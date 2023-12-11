enum {	SYMBOL_TYPE, 
	SCOPE_TYPE,
	ARGUMENT_LIST_TYPE,
	PERMUTE_LIST_TYPE,
	FOR_TYPE,
	GOTO_TYPE,
	LABEL_TYPE,
	RETURN_TYPE,
	RETURN_EXP_TYPE,
	CONTINUE_TYPE,
	BREAK_TYPE,
	WHILE_TYPE,
	IF_TYPE,
	IF_ELSE_TYPE,
	EQU_TYPE,
	TERN_TYPE,
	LOGIC_OR_TYPE,
	LOGIC_AND_TYPE,
	OR_TYPE,
	EOR_TYPE,
	AND_TYPE,
	EQU_EQU_TYPE,
	NOT_EQU_TYPE,
	LT_TYPE,
	GT_TYPE,
	LT_EQU_TYPE,
	GT_EQU_TYPE,
	LSH_TYPE,
	RSH_TYPE,
	SUB_TYPE,
	ADD_TYPE,
	MOD_TYPE,
	DIV_TYPE,
	MUL_TYPE,
	RUN_SUM_TYPE,
	RUN_DIF_TYPE,
	DEREF_TYPE,
	NEG_TYPE,
	LOGICAL_NOT_TYPE,
	NOT_TYPE,
	PERMUTE_TYPE,
	VEC_INDEX_TYPE,
	CALL_TYPE,
	CALL_PARAM_TYPE
};


enum {
	NONE_MOD = 0,	//space filler
	REF_MOD = 1,	//mark at 0 for a reference
	EXTERN_HINT = 2,
	GLOBAL_HINT = 3,
	CONST_HINT = 4,
	VOID_BASE = 5,
	BYTE_BASE = 6,
	WORD_BASE = 7,
	LONG_BASE = 8,
	QUAD_BASE = 9,	//also used to mark immediate integer data
	SINGLE_BASE = 10,
	DOUBLE_BASE = 11,	//also used to mark immediate floating data
	ARRAY_POSTFIX = 13,
	POINTER_POSTFIX = 14,
	FUNCTION_POSTFIX = 15,
	CLOSE_FUNCTION_POSTFIX = 16,
	SHARED_MOD = 17,
	VECTOR_MOD = 18,
	LABEL_BASE = 19
};

/*
 	modString of the form:

	HINT SHARED_MOD VECTOR_MOD SIZE BASETYPE            ARRAY_POSTFIX                               ... NONE_MOD
				                 SHARED_MOD POINTER_POSTFIX
				                            FUNCTION_POSTFIX ... CLOSE_FUNCTION_POSTFIX
	
	
*/

struct genericNode{
	long timestamp;
	char modString[32];
	long type;
	long childCount;
	struct genericNode* children[];		//something awful here: on a realoc, to expand chldrn list, you have to fix all the ptrs to this node...
};


//this one is always a fixed size
struct symbolEntry{
	long timestamp;				//last write
	char modString[32];
	char constValue[128];			//last const value this symbol was set too, this gets type punned
	char name[128];
	struct symbolEntry* baseStore;
	struct symbolEntry* next;
	struct genericNode* innerScope;		//the node the specifies the size / internal scope
	long stringLitBool;			//if the immediate is a string literal or not
};
