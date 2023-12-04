


enum {
	SYMBOL_TYPE, 
	SCOPE_TYPE,
	INIT_LIST_TYPE,
	DOTDOTDOT_TYPE,
	ARGUMENT_LIST_TYPE,
	PERMUTE_LIST_TYPE,
	FOR_TYPE,
	GOTO_TYPE,
	LABEL_TYPE,
	RETURN_TYPE,
	RETURN_EXP_TYPE,
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
	PUNN_TYPE,
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
	INDEX_TYPE
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
	DOTDOTDOT_BASE = 12,
	ARRAY_POSTFIX = 13,
	POINTER_POSTFIX = 14,
	FUNCTION_POSTFIX = 15,
	CLOSE_FUNCTION_POSTFIX = 16,
	SHARED_MOD = 17,
	VECTOR_MOD = 18,
	LABEL_BASE = 19
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

int dataLitType;
int dataLitIndex;

unsigned long currentScopeCounter;
unsigned long inStructBool;

struct symbolEntry* symbolStackPointer;
struct symbolEntry* symbolBasePointer;

unsigned long dagSize;
unsigned long dagStart;
struct genericNode** DAG;
unsigned long dagIndex;


static int globalTypeIndexStack[16];
static int globalTypePointer = 0;
static int falseScopeBool;


//0 = int, 1 = float
//size checking is left up to the tac to asm translator
void checkDataLitType(int in){
	if(in != dataLitType){
		fprintf(stderr, "ERR: TYPE MISMATCH IN DATA STRING LITERAL, LINE %ld\n", GLOBAL_LINE_NUMBER);
		exit(1);
	}
}

//used for building function (nested) types
//we need to save the pointer where we where in the last type string
void pushTypeIndex(){
	globalTypeIndexStack[globalTypePointer++] = globalTypeIndex;
}


//used to exit nested types
//we restore the pointer after appending the string we built
void copyAndPopTypeIndex(char* dest, char* src){

	int temp = globalTypeIndexStack[--globalTypePointer];

	for(int i = 0; i < globalTypeIndex; i++, temp++)
		dest[temp] = src[i];
	
	globalTypeIndex = temp;
}


//strict type compare
//pointer interact with quads and return pointers
//immediates can be sized up to match
//use left as type for dest, unless it's an immediate, then go right
void compareTypes(struct genericNode* a, struct genericNode* b, char *dest){
	
	int aType, bType;
	int aIndex, bIndex;

	for(int i = 31; i >= 0; i--)
	if( a->modString[i] != NONE_MOD ){
		aType = a->modString[i];		//get a's trailing type	
		aIndex = i;
		break;
	}

	for(int i = 31; i >= 0; i--)
	if( b->modString[i] != NONE_MOD ){
		bType = b->modString[i];		//get b's trailing type	
		bIndex = i;
		break;
	}

	if(aType < 0) //handle vector length bytes
		aType = a->modString[--aIndex];
	
	if(bType < 0) //handle vector length bytes
		bType = b->modString[--bIndex];


	switch(aType){							//ensure a is a valid type
		case (BYTE_BASE): break;
		case (WORD_BASE): break;
		case (LONG_BASE): break;
		case (QUAD_BASE): break;
		case (SINGLE_BASE): break;
		case (DOUBLE_BASE): break;
		case (POINTER_POSTFIX): break;
		case (VECTOR_MOD): goto VECTORCOMPARE;
		default: goto TYPEERROR;
	}

	switch(bType){							//ensure b is a valid type
		case (BYTE_BASE): break;
		case (WORD_BASE): break;
		case (LONG_BASE): break;
		case (QUAD_BASE): break;
		case (SINGLE_BASE): break;
		case (DOUBLE_BASE): break;
		case (POINTER_POSTFIX): break;
		case (VECTOR_MOD): goto TYPEERROR; //both a and b have to be vectors
		default: goto TYPEERROR;
	}


	//raw compare
	if( aType == bType ){
		memcpy(dest, a->modString, 32); //carry left forward
		return;
	}

	//pointer compare
	if( (aType == POINTER_POSTFIX && bType == QUAD_BASE) ){
		memcpy(dest, a->modString, 32); //carry left pointer forward
		return;
	}
	if( (bType == POINTER_POSTFIX && aType == QUAD_BASE) ){
		memcpy(dest, b->modString, 32); //carry right pointer forward
		return;
	}

	//constant (immediate derived) compare
	//test a for constant
	if( a->modString[0] == CONST_HINT )
	if( (aType <= bType) && (aType < SINGLE_BASE) && (bType < SINGLE_BASE) ){	//test to see if you can promote integers
		memcpy(dest, b->modString, 32);										//carry non immediate forward
		return;
	}else
	if( (aType <= bType) && (aType >= SINGLE_BASE) && (bType >= SINGLE_BASE) ){	//test to see if you can promote floats
		memcpy(dest, b->modString, 32);										//carry non immediate forward
		return;
	}

	//test b for constant
	if( b->modString[0] == CONST_HINT )
	if( (aType >= bType) && (aType < SINGLE_BASE) && (bType < SINGLE_BASE) ){	//test to see if you can promote integers
		memcpy(dest, a->modString, 32);										//carry non immediate forward
		return;
	}else
	if( (aType >= bType) && (aType >= SINGLE_BASE) && (bType >= SINGLE_BASE) ){	//test to see if you can promote floats
		memcpy(dest, a->modString, 32);										//carry non immediate forward
		return;
	}

	goto TYPEERROR; //don't bother with vector checking for scalers

VECTORCOMPARE:
	//vector compare
	if( a->modString[aIndex - 0] == b->modString[bIndex - 0] ) 		//compare sizes
	if( a->modString[aIndex - 1] == b->modString[bIndex - 1] ) 		//compare VECTOR_MOD
	if( a->modString[aIndex - 2] == b->modString[bIndex - 2] ){		//compare BASE type
		memcpy(dest, a->modString, 32); //carry left forward
		return;
	}

TYPEERROR:
	fprintf(stderr, "ERR: TYPE MISMATCH %d - %d, LINE %ld\n", aType, bType, GLOBAL_LINE_NUMBER);
	exit(1);
}





//make sure the argument types match whats expected (allow scalling consts up)
void checkArgumentTypes(struct genericNode* function, struct genericNode* param){

	//THIS NEEDS TO BE FIXED

	for(int i = 0; i < 32; i++)
		printf("%d:", function->modString[i]);
	printf("\n");


	//find i start
	int constFlag = 0;
	int i = 31;
	for(; i > -1; i--)
		if( function->modString[i] == FUNCTION_POSTFIX )
			break;

	for(int j = 0, w = 0; w < param->childCount; i++){
		
		if(param->children[w]->modString[j] == GLOBAL_HINT || param->children[w]->modString[j] == EXTERN_HINT)
			j++; //skip the hint section

		if(param->children[w]->modString[j] == CONST_HINT){
			constFlag = 1; //param is a constant
			j++;
		}

		if(param->children[w]->modString[j] == NONE_MOD){
			j = 0; 
			constFlag = 0; //reset flag
			w++; //next child
		}

		if(param->children[w]->modString[j] == function->modString[i]){
			j++;
			continue; //skip the fail fallthrough
		}

		//integer scale up
		if(constFlag)
		if(function->modString[i] < QUAD_BASE && function->modString[i] >= BYTE_BASE)
		if(param->children[w]->modString[j] < QUAD_BASE && param->children[w]->modString[j] >= BYTE_BASE)
		if(param->children[w]->modString[j] < function->modString[i]){
			j++;
			continue; //skip the fail fallthrough
		}

		//floating scale up
		if(constFlag)
		if(function->modString[i] == DOUBLE_BASE)
		if(param->children[w]->modString[j] == SINGLE_BASE){
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
void enforcePointer(struct genericNode* in){
	for(int i = 31; i > -1; i--){
		if(in->modString[i] == POINTER_POSTFIX){
			return;
		}

		if(in->modString[i] != NONE_MOD){
			fprintf(stderr, "ERR: TYPE MUST BE ARRAY OR POINTER, LINE %ld\n", GLOBAL_LINE_NUMBER);
			exit(1);
		}
	}
}


//ban floats and vectors
void enforceScalerInts(struct genericNode* in){
	for(int i = 31; i > -1; i--)
		if(	in->modString[i] == VECTOR_MOD || 
			in->modString[i] == POINTER_POSTFIX || 
			in->modString[i] == FUNCTION_POSTFIX || 
			in->modString[i] == SINGLE_BASE || 
			in->modString[i] == DOUBLE_BASE){
			fprintf(stderr, "ERR: TYPE MUST BE SCALER INTEGER, LINE %ld\n", GLOBAL_LINE_NUMBER);
			exit(1);
		}
}

//require vectors
void requireVecs(struct genericNode* in){
	for(int i = 0; i < 32; i--){
		if(in->modString[i] == VECTOR_MOD)		//vector mod
		if(in->modString[i + 1] < 0)			//vector length		
			return;
	}
	
	//error fallthrough
	fprintf(stderr, "ERR: TYPE MUST BE VECTOR, LINE %ld\n", GLOBAL_LINE_NUMBER);
	exit(1);
}


//enforce scope nesting is zero; because of a weird parsing order, items on the zero scope are marked as 1
void enforceZeroScope(){
	if(currentScopeCounter > 0){
		fprintf(stderr, "ERR: SCOPE MUST BE ZERO WAS %ld, LINE %ld\n", currentScopeCounter, GLOBAL_LINE_NUMBER);
		exit(1);
	}
}


//using type string, exclude functions
void excludeFunctionsType(char* in){
	for(int i = 31; i > -1; i--){
		if(in[i] == CLOSE_FUNCTION_POSTFIX || (in[i] == POINTER_POSTFIX && in[i - 1] == CLOSE_FUNCTION_POSTFIX)){
			fprintf(stderr, "ERR: TYPE MUST NOT BE FUNCTION OR FUNCTION POINTER, LINE %ld\n", GLOBAL_LINE_NUMBER);
			exit(1);
		}

		if(in[i] != NONE_MOD){
			return;
		}
	}
}


//using type string, force functions
void requireFunctionsType(char* in){

	for(int i = 0; i < 43; i++)
		printf("%d : ", in[i]);
	printf("\n");

	for(int i = 31; i > -1; i--){
		if(in[i] == CLOSE_FUNCTION_POSTFIX || (in[i] == POINTER_POSTFIX && in[i - 1] == CLOSE_FUNCTION_POSTFIX)){
			return;
		}

		if(in[i] != NONE_MOD){
			fprintf(stderr, "ERR: TYPE MUST BE FUNCTION OR FUNCTION POINTER, LINE %ld\n", GLOBAL_LINE_NUMBER);
			exit(1);
		}
	}
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

	int i = 0;
	for(; i < 32; i++)
		if(in->modString[i] < 0)
		if(in->modString[i - 1] == VECTOR_MOD)
			goto startShifting;


	fprintf(stderr, "ERR: TYPE MUST BE VECTOR, LINE %ld\n", GLOBAL_LINE_NUMBER);
	exit(1);


startShifting:
	for(int j = i - 1; j < 30; j++)
		in->modString[j] = in->modString[j + 2];

	in->modString[30] = NONE_MOD;
	in->modString[31] = NONE_MOD;

	return in;
}


//undo function/function pointer
struct genericNode* fetchFunc(struct genericNode* in){

	for(int i = 31; i > -1; i--){
		if(in->modString[i] == CLOSE_FUNCTION_POSTFIX){
			for(; in->modString[i] != FUNCTION_POSTFIX; i--)
				in->modString[i] = NONE_MOD;
			in->modString[i - 1] = NONE_MOD;
			return in;
		}

		if(in->modString[i] == POINTER_POSTFIX && in->modString[i - 1] == CLOSE_FUNCTION_POSTFIX){
			for(; in->modString[i] != FUNCTION_POSTFIX; i--)
				in->modString[i] = NONE_MOD;
			in->modString[i - 1] = NONE_MOD;
			return in;
		}

		if(in->modString[i] != NONE_MOD){
			fprintf(stderr, "ERR: TYPE MUST BE FUNCTION OR FUNCTION POINTER, LINE %ld\n", GLOBAL_LINE_NUMBER);
			exit(1);
		}
	}
}


//undo leftmost pointer and array mods
struct genericNode* fetchMod(struct genericNode* in){
	
	for(int i = 31; i >= 0; i--){
		if(in->modString[i] == POINTER_POSTFIX){
			in->modString[i] = NONE_MOD;

			return in;
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


// 0 = decimal, 1 = hex, 2 = binary, 3 = float, 4 = stringLiteral, 5 = label, 6 = dataStringLiteral
struct symbolEntry* createImmediate(char* inValue, int type){

	printf("create Immediate\n");

	static long stringLitCounter = 0;

	struct symbolEntry* temp = malloc(sizeof(struct symbolEntry));
	
	strcpy(temp->name, "0imm");
	memset(temp->modString, NONE_MOD, 32);
	temp->innerScope = NULL;
	temp->stringLitBool = 0;

	if(type < 3){
		strcpy(temp->constValue, inValue);

		long inputVal;
		
		if(type == 0)
			sscanf(inValue, "%ld", &inputVal);
		
		if(type == 1)
			sscanf(inValue, "%lx", &inputVal);
		
		if(type == 2)
			exit(1); //todo: add binary support


		free(inValue);

		//ugly as sin way of doing this
		if( inputVal < 128 && inputVal > -129)
			(temp->modString[0] = CONST_HINT, temp->modString[1] = BYTE_BASE);
		
		else if( inputVal < 256 && inputVal > -1)
			(temp->modString[0] = CONST_HINT, temp->modString[1] = BYTE_BASE);

		else if( inputVal < 32768 && inputVal > -32769)
			(temp->modString[0] = CONST_HINT, temp->modString[1] = WORD_BASE);

		else if( inputVal < 65536 && inputVal > -1)
			(temp->modString[0] = CONST_HINT, temp->modString[1] = WORD_BASE);

		else if( inputVal < 2147483648 && inputVal > -2147483649)
			(temp->modString[0] = CONST_HINT, temp->modString[1] = LONG_BASE);

		else if( inputVal < 4294967296 && inputVal > -1)
			(temp->modString[0] = CONST_HINT, temp->modString[1] = LONG_BASE);
		
		else
			(temp->modString[0] = CONST_HINT, temp->modString[1] = QUAD_BASE);

	}else if (type == 3){
		strcpy(temp->constValue, inValue);
		free(inValue);

		double inputVal;
		sscanf(inValue, "%lf", &inputVal);

		if(FLT_MAX <= inputVal && inputVal >= FLT_MIN)
			(temp->modString[0] = CONST_HINT, temp->modString[1] = SINGLE_BASE);
		else
			(temp->modString[0] = CONST_HINT, temp->modString[1] = DOUBLE_BASE);
	
	}else if (type == 4){
		temp->stringLitBool = 1;
		temp->modString[0] = CONST_HINT;
		temp->modString[1] = BYTE_BASE;
		temp->modString[2] = POINTER_POSTFIX;
		
		sprintf( &(temp->constValue)[3], "%ld", stringLitCounter++);
		
		temp->constValue[0] = 's';
		temp->constValue[1] = 't';
		temp->constValue[2] = 'r';

		temp->innerScope = (struct genericNode*)inValue;
	
	}else if (type == 6){
		temp->stringLitBool = 1;
		temp->modString[0] = CONST_HINT;
		temp->modString[1] = QUAD_BASE;
		temp->modString[2] = POINTER_POSTFIX;
		
		sprintf( &(temp->constValue)[3], "%ld", stringLitCounter++);
		
		temp->constValue[0] = 'l';
		temp->constValue[1] = 'i';
		temp->constValue[2] = 't';

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
	falseScopeBool = 0;
	dagSize = 0;
	dagStart = 0;
	dagIndex = 0;
	GLOBAL_LINE_NUMBER = 1;

	symbolStackPointer = malloc(sizeof(struct symbolEntry));
	
	symbolStackPointer->timestamp = 0;
	memset(symbolStackPointer->modString, NONE_MOD, 32);
	memset(symbolStackPointer->constValue, 0, 32);
	strcpy(symbolStackPointer->name, "0baseOfStack");
	symbolStackPointer->baseStore = NULL;
	symbolStackPointer->next = NULL;
	symbolStackPointer->innerScope = NULL;
	symbolStackPointer->stringLitBool = 0;	

	
	DAG = malloc(512 * sizeof(struct genericNode*));
	
	for(int i = 0; i < 512; i++)
		DAG[i] = NULL;

	dagSize = 512;
}


// take in an unregistered symbol, register it, produce a unregistered node; delay errs on goto labels
struct genericNode* registerSymbol(struct symbolEntry* in){

	printf("register Symbol\n");

	//see if it's an immediate
	long immediateCheck = 0;
	if(strcmp(in->name, "0imm") == 0)	//0imm is an invalid user symbol name, but it's the internal way of marking immediates
		immediateCheck = 1;

	printf("finish immediate check\n");

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
			
			memcpy(temp->modString, current->modString, 32); //copy the modString upwards
			
			for(int i = 31; i > -1; i--){
				if(temp->modString[i] == CLOSE_FUNCTION_POSTFIX)
					temp->modString[i + 1] = POINTER_POSTFIX; //handle function pointers
				
				if(temp->modString[i] != NONE_MOD)
					break;
			}

			free(in);	//get rid of the canidate Symbol
			return temp;
		}

		current = current->next;
	}

	printf("before BADCONST\n");

BADCONST:;

	printf("after BADCONST\n");

	//see if it's an invalid reference
	if(in->modString[0] == REF_MOD && in->modString[0] == LABEL_BASE){ //it's a label reference, mark it as in progress
		//otherwise register it, and make a node for it
		in->next = symbolStackPointer;
		symbolStackPointer = in;

		struct genericNode* temp = malloc( sizeof(struct genericNode) + sizeof(struct genericNode*) ); //create a pointer to it
		temp->childCount = 1;
		temp->type = SYMBOL_TYPE;
		temp->children[0] = (struct genericNode*)in;
	
		memcpy(temp->modString, current->modString, 32); //copy the modString upwards
	
		return temp;
	}else if(in->modString[0] == REF_MOD){ //it's a reference...
		printf("ERR: NON-EXISTANT SYMBOL: %s ON LINE %ld\n", in->name, GLOBAL_LINE_NUMBER); //...to something that doesn't exist
		exit(1);
	}

	printf("after REF\n");

	//otherwise register it, and make a node for it
	in->next = symbolStackPointer;
	symbolStackPointer = in;

	struct genericNode* temp = malloc( sizeof(struct genericNode) + sizeof(struct genericNode*) ); //create a pointer to it
	temp->childCount = 1;
	temp->type = SYMBOL_TYPE;
	temp->children[0] = (struct genericNode*)in;
	
	memcpy(temp->modString, in->modString, 32); //copy the modString upwards

	return temp;
}


void openScope(){
	symbolStackPointer->baseStore = symbolBasePointer;
	symbolBasePointer = symbolStackPointer;
	currentScopeCounter++;
	printf("OpenedScope\n");
}

void closeScope(){
	currentScopeCounter--;
	symbolStackPointer = symbolBasePointer;
	symbolBasePointer = symbolStackPointer->baseStore;
	printf("ClosedScope\n");
}




//opens a false scope to include function parameters, won't double up for things like functions that return functions
void openFalseScope(){

	if(falseScopeBool == 1)
		return;		//we already have a false scope open
		
	falseScopeBool = 1;
	symbolStackPointer->baseStore = symbolBasePointer;
	symbolBasePointer = symbolStackPointer;
	currentScopeCounter++;
}

//closes the false scope above
void closeFalseScope(){

	if(falseScopeBool == 0)
		return;		//we never opened a false scope

	falseScopeBool = 0;
	currentScopeCounter--;
	symbolStackPointer = symbolBasePointer;
	symbolBasePointer = symbolStackPointer->baseStore;
}


//assumption: nodes are registered one at a time
//this is likely true
//
//so we don't need to recursively compare(?)

static struct genericNode* nodeCompare(struct genericNode* a, struct genericNode* b){

	printf("compare nodes\n");

	if(a->childCount == b->childCount)
		if(memcmp(a, b, a->childCount * sizeof(struct genericNode*) + sizeof(struct genericNode)) == 0){  //they're the same
			for(int i = 0; i < b->childCount; i++) //check the children's timestamps
				if(b->children[i]->timestamp > b->timestamp) //timestamp error
					return NULL;
			

			return b; //otherwise it's a match
		}
	
	
	
	return NULL;
}

void clobberStores(){
	
	struct symbolEntry* current = symbolStackPointer;
	while(current != NULL){
		current->timestamp = globalTimestamp++;			//go through every symbol and mark them as being updated
		current = current->next;
	}

	return;
}



void endFunction(){
	dagStart = dagIndex;
}


//mode = 1 -> don't allow dangling function types; mode = 0 -> allow dangling function types
struct genericNode* registerNodeOperator(struct genericNode* in, int mode){

	printf("registerNode\n");

	in->timestamp = globalTimestamp++;

	for(int i = 31; i >= 0; i--)
		if(in->modString[i] != NONE_MOD){			
			if(in->modString[i] == CLOSE_FUNCTION_POSTFIX) //check to see if we have a dangling function
				if(mode){
					printf("ERR: DANGLING FUNCTION TYPE, LINE %ld\n", GLOBAL_LINE_NUMBER); //...to something that doesn't exist
					exit(1);
				}

		}else
			break;


	for(int i = dagIndex; i > dagStart; i--) //very important we try the newest first
		if(DAG[i] != NULL){

			struct genericNode* temp = NULL;
			long tempStamp = in->timestamp; //temporarly clobber timestamp
			in->timestamp = DAG[i]->timestamp;

			if( (temp = nodeCompare(in, DAG[i])) != NULL ){ //test for a match
				free(in);
				return temp; //found a match
			}
		
			in->timestamp = tempStamp; //restore the timestamp
		}


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

	printf("3\n");

	return in;
}


struct genericNode* registerNode(struct genericNode* in){
	return registerNodeOperator(in, 1);
}


struct genericNode* registerNodeFunction(struct genericNode* in){
	return registerNodeOperator(in, 0);
}




//adds a new child to a node, but since reallocing a node more often than not loses it's place in memory
//we have to go through and update all instances of it's old pointer
struct genericNode* appendAChild(struct genericNode* p, struct genericNode* c){

	printf("appending child\n");

	printf("children : %ld\n", p->childCount);

	if(p->childCount > 40000){
		printf("ahhhh\n");
		exit(1);
	}

	struct genericNode* newHome = malloc( sizeof(struct genericNode) + (sizeof(struct genericNode*) * (p->childCount + 1)) );

	newHome->timestamp = p->timestamp;
	memcpy(newHome->modString, p->modString, 32);
	newHome->type = p->type;
	newHome->childCount = p->childCount + 1;

	for(int i = 0; i < p->childCount; i++)
		newHome->children[i] = p->children[i];


	newHome->children[p->childCount] = c;		

	for(int i = 0; i < dagSize; i++)
		if(DAG[i] != NULL){
			if(DAG[i] == p)
				DAG[i] = newHome;
			else
				for(int j = 0; j < DAG[i]->childCount; j++)
					if(DAG[i]->children[j] == p)
						DAG[i]->children[j] = newHome;
		}


	free(p);
	return newHome;
}







































