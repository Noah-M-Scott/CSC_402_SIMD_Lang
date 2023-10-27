


enum {
	SYMBOL_TYPE, 
	SCOPE_TYPE,
	FOR_TYPE,
	GOTO_TYPE,
	LABEL_TYPE,
	RETURN_TYPE,
	DEFAULT_TYPE,
	CASE_TYPE,
	SWITCH_TYPE,
	CONTINUE_TYPE,
	BREAK_TYPE,
	DO_WHILE_TYPE,
	WHILE_TYPE,
	IF_TYPE,
	IF_ELSE_TYPE,
	COMMA_TYPE,
	SWAP_TYPE,
	EQU_TYPE,
	TERN_TYPE,
	LOGIC_OR_TYPE,
	LOGIC_AND_TYPE,
	MESH_TYPE,
	OR_TYPE,
	EOR_TYPE,
	AND_TYPE,
	EQU_EQU_TYPE,
	NOT_EQU_TYPE,
	EQU_EQU_TERN_TYPE,
	NOT_EQU_TERN_TYPE,
	LT_TYPE,
	GT_TYPE,
	LT_EQU_TYPE,
	GT_EQU_TYPE,
	LT_TERN_TYPE,
	GT_TERN_TYPE,
	LT_EQU_TERN_TYPE,
	GT_EQU_TRN_TYPE,
	LSH_TYPE,
	RSH_TYPE,
	SUB_TYPE,
	ADD_TYPE,
	MOD_TYPE,
	DIV_TYPE,
	MUL_TYPE,
	PUNN_TYPE,
	CONV_TYPE,
	SUM_TYPE,
	DEREF_TYPE,
	REF_TYPE,
	NEG_TYPE,
	ABS_TYPE,
	LOGICAL_NOT_TYPE,
	NOT_TYPE,
	BLOCK_TYPE,
	ARROW_TYPE,
	DOT_TYPE,
	PERMUTE_TYPE,
	CALL_TYPE,
	CALL_PARAM_TYPE,
	INDEX_TYPE
};


enum {
	EXTERN_HINT,
	GLOBAL_HINT,
	VOID_BASE,
	BYTE_BASE,
	WORD_BASE,
	LONG_BASE,
	QUAD_BASE,
	APPROX_BASE,
	REAL_BASE,
	STRUCT_BASE,
	U_PREFIX,
	ARRAY_POSTFIX,
	POINTER_POSTFIX,
	FUNCTION_POSTFIX,
	CONST_MOD,
	SHARED_MOD
};


/*
	A note on timestamps:

	timestamps note when a node was last updated, all nodes are marked on completion of creation,
	but variables are updated on every write back.

	to use an expression, it first must be traced back to it's root, if at any point
	prev.timestamp < next.timestamp, we must re compute the branch prev-up.
	

	The leftmost bit of timestamp is a dirtybit for the DAG to TAC converter
*/

struct genericNode{
	long type;
	long timestamp;
	long nodeSize;
	long childCount;
	struct genericNode* children[];
};


struct symbolNode{
	long type;	//must be SYMBOL_TYPE
	long timestamp;
	long nodeSize;
	long constValue;	//last const value this symbol was set too
	struct genericNode* size;
	char modString[32];
	struct scopeNode* scope;
	char name[];
};




long globalTimestamp;
unsigned long currentTableSize;
struct opNode** symbolTable;

void initNodes(){
	globalTimestamp = 0;
	currentTableSize = 0;
	symbolTable = malloc(512 * sizeof(struct genericNode*));
	for(;currentTableSize < 512; currentTableSize++)
		symbolTable[currentTableSize] = NULL;

	currentTableSize++;
}

struct genericNode* registerNode(struct genericNode* in){
	static unsigned long symbolIndex = 0;
	
	if(in.type == SYMBOL_TYPE)
		;	//we want to handle symbols seperately


	int temp = NULL;
	for(int i = currentTableSize - 1; i >= 0; i--)
		if(symbolTable[i] != NULL)
			if( (temp = recursiveCompare(in, symbolTable[i])) != NULL )
				return temp; //found a match

	
	//couldn't find a match
	int newSize = 0;
	symbolTable[symbolIndex++] = in;
	if(symbolIndex == currentTableSize){
		newSize = currentTableSize << 1;
		symbolTable = realloc(symbolTable, newSize * sizeof(struct genericNode*));
		for(; currentTableSize < newSize; currentTableSize++)
			symbolTable[currentTableSize] = NULL;
		currentTableSize++;
	}
	
	return in;
}



struct genericNode* recursiveCompare(struct genericNode* a, struct genericNode* b){
	
	if(a->nodeSize == b->nodeSize)
		if(!memcmp(a, b, a->nodeSize))
			for(int i = 0; i < a->childCount; i++){
				if(a->children[i]->type == SYMBOL_TYPE)
					//compare timestamps, might need to rebuild whole branch, otherwise, this is a end
				else
					return recursiveCompare(a->children[i], b->children[i]);
			}
	return NULL;
}



















