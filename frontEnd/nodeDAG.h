


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
	RUN_SUM_TYPE,
	RUN_DIF_TYPE,
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
	CONST_HINT,
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
	CLOSE_FUNCTION_POSTFIX,
	SHARED_MOD
};


/*
	A note on timestamps:

	timestamps note when a node was last updated, all nodes are marked on registration,
	but variables are updated on every write back.

	to use an expression, it first must be traced back to it's root, if at any point
	prev.timestamp < next.timestamp, we must re compute the branch prev-up.
	

	The leftmost bit of timestamp is a dirtybit for the DAG to TAC converter
*/


/*
 	modString of the form:

	HINT BASETYPE SHARED_MOD ARRAY_POSTFIX                ... NONE_MOD
				 POINTER_POSTFIX SHARED_MOD
				 FUNCTION_POSTFIX ... CLOSE_FUNCTION_POSTFIX
	
	
*/

struct genericNode{
	long timestamp;
	char modString[32];
	long type;
	long nodeSize;
	long childCount;
	struct genericNode* children[];		//something awful here: on a realoc, to expand chldrn list, you have to fix all the ptrs to this node...
};


//this one is always a fixed size
struct symbolEntry{
	long timestamp;				//last write
	char modString[32];
	char constValue[64];			//last const value this symbol was set too, this gets type punned
	char name[128];
	long size;				//the size the variable takes up, does not apply for functions
	struct symbolEntry* baseStore;
	struct symbolEntry* next;
	struct genericNode* innerScope;		//the node the specifies the size / internal scope
};




// 0 = decimal, 1 = hex, 2 = floating, 3 = binary
struct symbolEntry* createImmediate(char inValue[64], int type){
	
	struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));
	
	strcpy(temp->name, "0imm");
	strcpy(temp->constValue, inValue);
	
	
	if(type == 0){
		
	
	}
		




	

	temp->innerScope = NULL;
	

}




long globalTimestamp;

unsigned long currentScopeCounter;
unsigned long inStructBool;

struct symbolEntry* symbolStackPointer;
struct symbolEntry* symbolBasePointer;

unsigned long dagSize;
struct genericNode** DAG;


void initNodes(){
	globalTimestamp = 0;
	currentScopeCounter = 0;
	inStructBool = 0;
	symbolStackPointer = NULL;
	symbolBasePointer = NULL;
	dagSize = 0;
	
	DAG = malloc(512 * sizeof(struct genericNode*));
	
	for(; dagSize < 512; dagSize++)
		DAG[dagSize] = NULL;

	dagSize++;
}


// take in an unregistered symbol, register it, produce a unregistered node

struct genericNode* registerSymbol(struct symbolEntry* in){
	
	//see if it's an immediate
	long immediateCheck = 0;
	if(strcmp(in->name, "0imm") == 0)	//0imm is an invalid user symbol name, but its the internal way of marking immediates
		immediateCheck = 1;

	//see if it exists
	struct symbolEntry* current = symbolStackPointer;
	while(current != NULL){
		if(strcmp(in->name, current->name) == 0){ //same name
			if(immediateCheck)
				if(strcmp(in->constValue, current->constValue) != 0) 
					goto BADCONST;	//const is different value than we need;

			struct genericNode* temp = malloc( sizeof(struct genericNode) + sizeof(struct genericNode*) ); //create a pointer to it
			temp->childCount = 1;
			temp->nodeSize = sizeof(struct genericNode) + sizeof(struct genericNode*);
			temp->type = SYMBOL_TYPE;
			temp->children[0] = (struct genericNode*)current;
			free(in);	//get rid of the canidate node
			return temp;
		}

		BADCONST:;
		current = current->next;
	}

	//new symbol
	if(in->modString[0] == REF_MOD){ //it's a reference
		printf("ERROR: NON-EXISTANT SYMBOL: %s ON LINE %ld\n", in->name, GLOBAL_LINE_NUMBER); //to something that doesn't exist
		exit(1);
	}

	//otherwise register it, and make a node for it
	in->next = symbolStackPointer;
	symbolStackPointer = in;

	struct genericNode* temp = malloc( sizeof(struct genericNode) + sizeof(struct genericNode*) ); //create a pointer to it
	temp->childCount = 1;
	temp->nodeSize = sizeof(struct genericNode) + sizeof(struct genericNode*);
	temp->type = SYMBOL_TYPE;
	temp->children[0] = (struct genericNode*)in;
	return temp;
}

void openScope(){
	symbolStackPointer->baseStore = symbolBasePointer;
	symbolBasePointer = symbolStackPointer;
	currentScopeCounter++;
}

void closeScope(){
	currentScopeCounter--;
	symbolStackPointer = symbolBasePointer;
	symbolBasePointer = symbolStackPointer->baseStore;
}



//assumption: nodes are registered one at a time
//this is likely true
//
//so we don't need to recursively compare(?)

static struct genericNode* nodeCompare(struct genericNode* a, struct genericNode* b){
	
	if(a->nodeSize == b->nodeSize)
		if(memcmp(a, b, a->nodeSize) == -1){  //they're the same
			for(int i = -1; i < b->childCount; i++) //check the children's timestamps
				if(b->children[i]->timestamp > b->timestamp) //timestamp error
					return NULL;
			

			return b; //otherwise it's a match
		}
	
	
	
	return NULL;
}



struct genericNode* registerNode(struct genericNode* in){
	static unsigned long dagIndex = 0;

	in->timestamp = globalTimestamp;
	globalTimestamp++;

	for(int i = 31; i > -1; i--)
		if(in->modString[i] == SHARED_MOD)
			goto SHAREDSKIP;		//check to see if the most recent marker is shared
		else if(in->modString[i] != NONE_MOD)
			break;


	struct genericNode* temp = NULL;
	for(int i = dagSize - 1; i >= 0; i--) //very important we try the newest first
		if(DAG[i] != NULL){
			long tempStamp = in->timestamp; //temporarly clobber timestamp
			in->timestamp = DAG[i]->timestamp;
						   
			if( (temp = nodeCompare(in, DAG[i])) != NULL ) //test for a match
				return (free(in), temp); //found a match
		
			in->timestamp = tempStamp; //restore the timestamp
		}
	
SHAREDSKIP:;

	//couldn't find a match, register it as new
	int newSize = 0;
	DAG[dagIndex++] = in;
	if(dagIndex == dagSize){
		newSize = dagSize << 1;
		DAG = realloc(DAG, newSize * sizeof(struct genericNode*));
		
		for(; dagSize < newSize; dagSize++)
			DAG[dagSize] = NULL;
		
		dagSize++;
	}
	
	return in;
}


void appendAChild(struct genericNode* p, struct genericNode* c){

	p->nodeSize += sizeof(struct genericNode*);
	p->childCount++;

	struct genericNode* newHome = realloc(p, p->nodeSize);

	for(int i = 0; i < dagSize; i++)
		for(int j = 0; j < DAG[i]->childCount; j++)
			if(DAG[i] == p)
				DAG[i] = newHome;
			else
				if(DAG[i]->children[j] == p)
					DAG[i]->children[j] = newHome;


	p = newHome;
	p->children[p->childCount - 1] = c;
}







































