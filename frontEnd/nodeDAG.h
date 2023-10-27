


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
	NONE_MOD,	//space filler
	REF_MOD,	//mark at 0 for a reference
	EXTERN_HINT,
	GLOBAL_HINT,
	VOID_BASE,
	BYTE_BASE,	//note: don't bother to type check unsigned-ness, let's me do -1 for 0xFF trick
	WORD_BASE,
	LONG_BASE,
	QUAD_BASE,	//also used to mark immediate integer data
	SINGLE_BASE,
	DOUBLE_BASE,	//also used to mark immediate floating data
	STRUCT_BASE,
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


//this one is always a fixed size
struct symbolEntry{
	long scopeDepth;			//how deep in the scope it is
	long timestamp;				//last write
	long long constValue;			//last const value this symbol was set too, this gets type punned
	struct genericNode* sizeAndScope;	//the node the specifies the size / scope
	char modString[32];
	char name[128];
};


long globalTimestamp;

unsigned long currentScopeCounter;

unsigned long symbolTableIndex;
struct symbolEntry symbolTable[512];


unsigned long dagSize;
struct genericNode** DAG;


void initNodes(){
	globalTimestamp = 0;
	symbolTableIndex = 0;
	currentScopeCounter = 0;
	dagSize = 0;
	
	DAG = malloc(512 * sizeof(struct genericNode*));
	
	for(; dagSize < 512; dagSize++)
		DAG[dagSize] = NULL;

	dagSize++;
}


struct genericNode* registerSymbol(struct symbolEntry* in){
	
	long immediateCheck = 0;
	if(strcmp(in->name, "0immediate") == 0)	//0immediate is an invalid name, so it's a safe internal only
		immediateCheck = 1;


	for(i = symbolTableIndex - 1; i >= 0; i--){
		if(strcmp(in->name, symbolTable[symbolTableIndex]->name) == 0){
			if(immediateCheck)
				if(in->constValue != symbolTable[symbolTableIndex]->constValue)
					goto its12o4atnight;	//const is different value than we need;

			struct genericNode* temp = malloc( sizeof(struct genericNode) + sizeof(struct genericNode*) ) //create a pointer to it
			temp->childCount = 1;
			temp->nodeSize = sizeof(struct genericNode) + sizeof(struct genericNode*);
			temp->type = SYMBOL_TYPE;
			temp->children[0] = symbolTable[symbolTableIndex];
			free(in);
			return temp;
		}
		its12o4atnight:
	}

	//new symbol
	if(in->modString[0] == REF_MOD){ //it's a reference
		printf("ERROR: NON-EXISTANT SYMBOL: %s ON LINE %d\n", in.name, GLOBAL_LINE_NUMBER)//to something that doesn't exist
		exit(1);
	}

	//otherwise register it, and make a node for it


}

void pushScope(){
	currentScopeCounter++;
}

void popScope(){
	while(symbolTable[symbolTableIndex - 1]->scopeDepth == currentScopeDepth){
		free(symbolTable[symbolTableIndex - 1]);      
		symbolTableIndex--;
	}
	currentScopeCounter--;
}




struct genericNode* registerNode(struct genericNode* in){
	static unsigned long dagIndex = 0;

	in.timestamp = globalTimestamp;
	globalTimestamp++;

	int temp = NULL;
	for(int i = dagSize - 1; i >= 0; i--) //very important we try the newest first
		if(DAG[i] != NULL)
			if( (temp = recursiveCompare(in, DAG[i])) != NULL )
				return (free(in), temp); //found a match

	
	//couldn't find a match, register it as new
	int newSize = 0;
	DAG[dagIndex++] = in;
	if(dagIndex == dagSize){
		newSize = dagSize << 1;
		DAG = realloc(DAG, newSize * sizeof(struct genericNode*));
		
		for(; dagSize < newSize; dagSize++)
			DAG[dagSize] = NULL;
		
		currentTableSize++;
	}
	
	return in;
}



struct genericNode* recursiveCompare(struct genericNode* a, struct genericNode* b){
	
	if(a->nodeSize == b->nodeSize)
		if(memcmp(a, b, a->nodeSize) == 0){
			for(int i = 0; i < a->childCount; i++)
				if(a->children[i]->type == SYMBOL_TYPE)
					if(symbolTable[a->children[i]->children[0]].timestamp > b->children[i].timestamp)
						return NULL //the cache here is bum, we need to redo calculations
					
					//otherwise, fall through and continue tests
				else{
					struct genericNode* temp = recursiveCompare(a->children[i], b->children[i]);
					if(temp != b->children[i])
						return NULL;
				}
			return b;
		}
	
	
	
	return NULL;
}



















