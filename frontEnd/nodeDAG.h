


enum {
	SYMBOL_TYPE, 
	SCOPE_TYPE,
	INIT_LIST_TYPE,
	DOTDOTDOT_TYPE,
	ARGUMENT_LIST_TYPE,
	PERMUTE_LIST_TYPE,
	DEC_LIST_TYPE,
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
	ULT_TYPE,
	UGT_TYPE,
	ULT_EQU_TYPE,
	UGT_EQU_TYPE,
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
	NEG_TYPE,
	ABS_TYPE,
	LOGICAL_NOT_TYPE,
	NOT_TYPE,
	PERMUTE_TYPE,
	VEC_INDEX_TYPE,
	CALL_TYPE,
	CALL_PARAM_TYPE,
	INDEX_TYPE,
	AMP_INDEX_TYPE
};


enum {
	NONE_MOD,	//space filler
	REF_MOD,	//mark at 0 for a reference
	EXTERN_HINT,
	GLOBAL_HINT,
	CONST_HINT,
	VOID_BASE,
	BYTE_BASE,
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
	SHARED_MOD,
	VECTOR_MOD
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

	HINT SHARED_MOD VECTOR_MOD SIZE BASETYPE            ARRAY_POSTFIX                               ... NONE_MOD
				                 SHARED_MOD POINTER_POSTFIX
				                            FUNCTION_POSTFIX ... CLOSE_FUNCTION_POSTFIX
	
	
*/


/*
 *	BIG IMPORTANT NOTE: IF YOUR GETTING ERRORS, ENSURE THAT STRUCT PACKING IS OFF ON COMPILE; THIS USES SOME PSEUDO POLYMORPHISM
 *	ON "timestamp"
 *
 *	THOUGH IT SHOULD BE FINE EVEN IF IT'S ON
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
	long size;				//the size the variable takes up, does not apply for functions
	struct symbolEntry* baseStore;
	struct symbolEntry* next;
	struct genericNode* innerScope;		//the node the specifies the size / internal scope
	long stringLitBool;			//if the immediate is a string literal or not
};


unsigned long inStructBool;
unsigned long inFunctionParamBool;
unsigned long poisonRefBool;
unsigned long globalRunningSize;
int globalTypeIndex;
char globalIPT[32];
long globalTimestamp;

unsigned long currentScopeCounter;
unsigned long inStructBool;

struct symbolEntry* symbolStackPointer;
struct symbolEntry* symbolBasePointer;

unsigned long dagSize;
struct genericNode** DAG;



void pushTypeIndex(){

}


void copyAndPopTypeIndex(char* dest, char* src){

}



//strict type compare (allow scalling ints up)
void compareTypes(struct genericNode* a, struct genericNode* b){
	
	int hintSkipA = 0;
	int hintSkipB = 0;

	if( a->modString[0] == GLOBAL_HINT || a->modString[0] == EXTERN_HINT || a->modString[0] == CONST_HINT )
		hintSkipA = 1;

	if( b->modString[0] == GLOBAL_HINT || b->modString[0] == EXTERN_HINT || b->modString[0] == CONST_HINT )
		hintSkipB = 1;

	if( memcmp(&(a->modString)[hintSkipA], &(b->modString)[hintSkipB], 31) == 0 )
		return;

	fprintf(stderr, "ERR: TYPE MISMATCH, LINE %ld\n", GLOBAL_LINE_NUMBER);
	exit(1);
}


//make sure the argument types match whats expected (allow scalling ints up)
void checkArgumentTypes(struct genericNode* function, struct genericNode* param){


	//find i start
	int i = 31;
	for(; i > -1; i--)
		if( function->modString[i] == FUNCTION_POSTFIX )
			break;

	for(int j = 0, w = 0; w < param->childCount; i++){
		
		if(param->children[w]->modString[j] == GLOBAL_HINT || param->children[w]->modString[j] == EXTERN_HINT || param->children[w]->modString[j] == CONST_HINT)
			j++; //skip the hint section

		if(pram->children[w]->modString[j] == NONE_TYPE){
			j = 0; 
			w++; //next child
		}

		if(param->children[w]->modString[j] == function->modString[i]){
			j++;
			continue; //skip the fail fallthrough
		}

		//fail fallthrough
		fprintf(stderr, "ERR: ARGUMENT MISMATCH, LINE %ld\n", GLOBAL_LINE_NUMBER);
		exit(1);
	}
}

//make sure vecs are same length
void checkLengths(struct genericNode* a, struct genericNode* b){
	
	char lA = 0;
	char lB = 0;

	for(int i = 0; i < 32; i++)
		if( a->modString[i] == VECTOR_MOD )
			lA = a->modString[i + 1];
		else if( b->modString[i] == VECTOR_MOD )
			lB = b->modString[i + 1];
	
	if(lA == lB)
		return;
	
	//fail fallthrough
	fprintf(stderr, "ERR: VECTOR LENGTH MISMATCH, LINE %ld\n", GLOBAL_LINE_NUMBER);
	exit(1);
}


//enforce pointer/array
void enforcePointer(){

}

//ban floats and vectors
void enforceScalerInts(){

}

//require vectors
void requireVecs(){

}

//enforce scope nesting is zero
void enforceZeroScope(){

}



void excludeFunctionsType(char* in){

}


void requireFunctionsType(char* in){


}



//make sure the type isn't a float
void excludeFloats(struct genericNode* in){

	for(int i = 0; i < 32; i++)
		if(in->modString[i] == SINGLE_BASE || in->modString[i] == DOUBLE_BASE){
			fprintf(stderr, "ERR: FLOAT USED WHERE FLOATS ARE BANNED, LINE %ld\n", GLOBAL_LINE_NUMBER);
			exit(1);	
		}

}


//require most recent mod to be function
void requireFunctions(struct genericNode* in){

	for(int i = 31; i > -1; i--){
		if(in->modString[i] == CLOSE_FUNCTION_POSTFIX || (in->modString[i] == POINTER_POSTFIX && in->modString[i - 1] == CLOSE_FUNCTION_POSTFIX)){
			return;
		}

		if(in->modString[i] != NONE_MOD){
			fprintf(stderr, "ERR: TYPE MUST BE FUNCTION OR FUNCTION POINTER, LINE %ld\n", GLOBAL_LINE_NUMBER);
			exit(1);
		}
	}
}


//undo the vector
struct genericNode* undoVector(struct genericNode* in){


}


//undo function/function pointer
struct genericNode* fetchFunc(struct genericNode* in){

	for(int i = 31; i > -1; i--){
		if(in->modString[i] == CLOSE_FUNCTION_POSTFIX){
			for(; in->modString[i] != FUNCTION_POSTFIX; i--)
				in->modString[i] = NONE_MOD;
			in->modString[i - 1] = NONE_MOD;
			return;
		}

		if(in->modString[i] == POINTER_POSTFIX && in->modString[i - 1] == CLOSE_FUNCTION_POSTFIX){
			for(; in->modString[i] != FUNCTION_POSTFIX; i--)
				in->modString[i] = NONE_MOD;
			in->modString[i - 1] = NONE_MOD;
			return;
		}

		if(in->modString[i] != NONE_MOD){
			fprintf(stderr, "ERR: TYPE MUST BE FUNCTION OR FUNCTION POINTER, LINE %ld\n", GLOBAL_LINE_NUMBER);
			exit(1);
		}
	}
}


//undo leftmost pointer and array mods
struct genericNode* fetchMod(struct genericNode* in){
	
	for(int i = 31; i > -1; i--){
		if(in->modString[i] == POINTER_POSTFIX || in->modString[i] == ARRAY_POSTFIX){
			in->modString[i] = NONE_MOD;
			return;
		}

		if(in->modString[i] != NONE_MOD){
			fprintf(stderr, "ERR: TYPE CANNOT BE DEREFERENCED, LINE %ld\n", GLOBAL_LINE_NUMBER);
			exit(1);
		}
	}
}





//creates a incomplete symbol node, marks a ref
struct symbolEntry* createRef(char* inName){

	struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));
	strcpy(temp->name, inName);
	memset(temp->modString, NONE_MOD, 32);
	temp->modString[0] = REF_MOD;
	
	return temp;
}


// 0 = decimal, 1 = hex, 2 = binary, 3 = float, 4 = stringLiteral, 5 = label
struct symbolEntry* createImmediate(char* inValue, int type){
	static long stringLitCounter = 0;

	struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));
	
	strcpy(temp->name, "0imm");
	memset(temp->modString, NONE_MOD, 32);
	temp->innerScope = NULL;
	temp->stringLitBool = 0;

	if(type < 3){
		strcpy(temp->constValue, inValue);
		free(inValue);

		long inputVal;
		
		if(type == 0)
			sscanf(inValue, "%ld", &inputVal);
		
		if(type == 1)
			sscanf(inValue, "%lx", &inputVal);
		
		if(type == 2)
			exit(1); //todo: add binary support
		

		//ugly as sin way of doing this
		if( inputVal < 128 && inputVal > -129)
			(temp->size = 1, temp->modString[0] = CONST_HINT, temp->modString[1] = BYTE_BASE);

		else if( inputVal < 256 && inputVal > -1)
			(temp->size = 1, temp->modString[0] = CONST_HINT, temp->modString[1] = BYTE_BASE);

		else if( inputVal < 32768 && inputVal > -32769)
			(temp->size = 2, temp->modString[0] = CONST_HINT, temp->modString[1] = WORD_BASE);

		else if( inputVal < 65536 && inputVal > -1)
			(temp->size = 2, temp->modString[0] = CONST_HINT, temp->modString[1] = WORD_BASE);

		else if( inputVal < 2147483648 && inputVal > -2147483649)
			(temp->size = 4, temp->modString[0] = CONST_HINT, temp->modString[1] = LONG_BASE);

		else if( inputVal < 4294967296 && inputVal > -1)
			(temp->size = 4, temp->modString[0] = CONST_HINT, temp->modString[1] = LONG_BASE);
		
		else
			(temp->size = 8, temp->modString[0] = CONST_HINT, temp->modString[1] = QUAD_BASE);
	
	}else if (type == 3){
		strcpy(temp->constValue, inValue);
		free(inValue);

		double inputVal;
		sscanf(inValue, "%lf", &inputVal);

		if(FLT_MAX <= inputVal && inputVal >= FLT_MIN)
			(temp->size = 4, temp->modString[0] = CONST_HINT, temp->modString[1] = SINGLE_BASE);
		else
			(temp->size = 8, temp->modString[0] = CONST_HINT, temp->modString[1] = DOUBLE_BASE);
	
	}else if (type == 4){
		temp->stringLitBool = 1;
		temp->modString[0] = CONST_HINT;
		temp->modString[1] = BYTE_BASE;
		temp->modString[2] = POINTER_POSTFIX;
		temp->size = 8;
		
		sprintf( &(temp->constValue)[3], "%ld", stringLitCounter++);
		
		temp->constValue[0] = 's';
		temp->constValue[1] = 't';
		temp->constValue[2] = 'r';

		temp->innerScope = (struct genericNode*)inValue;
	
	}else{
		//create label immediate
		strcpy( &(temp->name)[1], inValue );
		
		temp->name[0] = '&';

		strcpy( temp->constValue, inValue );
		
		//find name to copy modString, append a star; but don't support functions
		struct symbolEntry* current = symbolStackPointer;
		while(current != NULL){
			if(strcmp(inValue, current->name) == 0){ //same name			
			
				memcpy(temp->modString, current->modString, 32);
				
				for(int i = 30; i > -1; i--)
					if(temp->modString[i] != NONE_MOD){
						temp->modString[i + 1] = POINTER_POSTFIX;
						break;
					}
				

				return temp;

			}

			current = current->next;
		}

		fprintf(stderr, "ERROR: LABEL DOES NOT EXIST, LINE %ld\n", GLOBAL_LINE_NUMBER);
		exit(1);
	}
		

	return temp;
}





void initNodes(){
	globalTimestamp = 0;
	currentScopeCounter = 0;
	inStructBool = 0;
	poisonRefBool = 0;
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
	
	//TODO: have a special thing for function pointers

	//see if it's an immediate
	long immediateCheck = 0;
	if(strcmp(in->name, "0imm") == 0)	//0imm is an invalid user symbol name, but it's the internal way of marking immediates
		immediateCheck = 1;

	//see if it exists
	struct symbolEntry* current = symbolStackPointer;
	while(current != NULL){
		if(strcmp(in->name, current->name) == 0){ //same name
			
			if(immediateCheck){ //see if it's an immediate we already have in the DAG
				if(strcmp(in->constValue, current->constValue) != 0) 
					goto BADCONST;	//const is different value than we need;
			
			}else if(in->modString[0] != REF_MOD){ //it's a not a immediate, nor a reference, double define
				fprintf(stderr, "ERROR: DOUBLE DEFINE: %s, ON LINE %ld\n", in->name, GLOBAL_LINE_NUMBER);
				exit(1);
			
			}

			//otherwise it's a valid ref, swap it out with the right symbol
			struct genericNode* temp = malloc( sizeof(struct genericNode) + sizeof(struct genericNode*) ); //create a pointer to it
			temp->childCount = 1;
			temp->type = SYMBOL_TYPE;
			temp->children[0] = (struct genericNode*)current;
			
			strcpy(temp->modString, current->modString); //copy the modString upwards
			
			if(poisonRefBool && !immediateCheck) //this is a poison reference to a non immediate, and the timestamp on the symbol needs to be updated
				current->timestamp = globalTimestamp++;


			free(in);	//get rid of the canidate Symbol
			return temp;
		}

		current = current->next;
	}

BADCONST:;

	//see if it's an invalid reference
	if(in->modString[0] == REF_MOD){ //it's a reference...
		printf("ERROR: NON-EXISTANT SYMBOL: %s ON LINE %ld\n", in->name, GLOBAL_LINE_NUMBER); //...to something that doesn't exist
		exit(1);
	}


	//otherwise register it, and make a node for it
	in->next = symbolStackPointer;
	symbolStackPointer = in;

	struct genericNode* temp = malloc( sizeof(struct genericNode) + sizeof(struct genericNode*) ); //create a pointer to it
	temp->childCount = 1;
	temp->type = SYMBOL_TYPE;
	temp->children[0] = (struct genericNode*)in;
	
	strcpy(temp->modString, current->modString); //copy the modString upwards
	
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



void openFalseScope(){
	symbolStackPointer->baseStore = symbolBasePointer;
	symbolBasePointer = symbolStackPointer;
	currentScopeCounter++;
}

void closeFalseScope(){
	currentScopeCounter--;
	symbolStackPointer = symbolBasePointer;
	symbolBasePointer = symbolStackPointer->baseStore;
}


//assumption: nodes are registered one at a time
//this is likely true
//
//so we don't need to recursively compare(?)

static struct genericNode* nodeCompare(struct genericNode* a, struct genericNode* b){
	
	if(a->childCount == b->childCount)
		if(memcmp(a, b, a->childCount * sizeof(struct genericNode*) + sizeof(struct genericNode)) == 0){  //they're the same
			for(int i = 0; i < b->childCount; i++) //check the children's timestamps
				if(b->children[i]->timestamp > b->timestamp) //timestamp error
					return NULL;
			

			return b; //otherwise it's a match
		}
	
	
	
	return NULL;
}


struct genericNode* registerNodeFunction(struct genericNode* in){

}


struct genericNode* registerNode(struct genericNode* in){
	static unsigned long dagIndex = 0;

	in->timestamp = globalTimestamp++;

	for(int i = 31; i > 0; i--)
		if(in->modString[i] != NONE_MOD){
			if(in->modString[i - 1] == SHARED_MOD)
				goto SHAREDSKIP;		//check to see if the most recent marker is shared
		}else
			break;


	struct genericNode* temp = NULL;
	for(int i = dagIndex; i > -1; i--) //very important we try the newest first
		if(DAG[i] != NULL){
			long tempStamp = in->timestamp; //temporarly clobber timestamp
			in->timestamp = DAG[i]->timestamp;
						   
			if( (temp = nodeCompare(in, DAG[i])) != NULL ) //test for a match
				return (free(in), temp); //found a match
		
			in->timestamp = tempStamp; //restore the timestamp
		}
	
SHAREDSKIP:;

	//either couldn't find a match, register it as new; or the node is shared and needs to be redone
	int newSize = 0;
	DAG[dagIndex++] = in;

	if(dagIndex == dagSize){		//realloc if needed
		newSize = dagSize * 2;
		DAG = realloc(DAG, newSize * sizeof(struct genericNode*));
		
		for(; dagSize < newSize; dagSize++)
			DAG[dagSize] = NULL;		//null out the empty ones just in case
		
		dagSize++;
	}
	
	return in;
}


//adds a new child to a node, but since reallocing a node more often than not loses it's place in memory
//we have to go through and update all instances of it's old pointer
struct genericNode* appendAChild(struct genericNode* p, struct genericNode* c){

	p->childCount++;

	struct genericNode* newHome = realloc(p, p->nodeSize);

	for(int i = 0; i < dagSize; i++)
		if(DAG[i] != NULL){
			if(DAG[i] == p)
				DAG[i] = newHome;
			else
				for(int j = 0; j < DAG[i]->childCount; j++)
					if(DAG[i]->children[j] == p)
						DAG[i]->children[j] = newHome;
		}


	newHome->children[p->childCount - 1] = c;
	return newHome;
}







































