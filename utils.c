#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//struct of any list's node, generally nodes are considered in last-first order
typedef struct node {
    char *factor;
    struct node *next;
} node_t;

typedef struct nodeInt {
    int data;
    struct Node* next;
} node_int_t;


void free_list(node_t* head) {
	node_t* tmp;
	char *fact;

	while (head != NULL) {
		tmp = head;
		head = head->next;
		fact = tmp->factor;
		//free(tmp->factor);
		tmp->factor = NULL;
		tmp->next = NULL;
		free(tmp);
		free(fact);
		fact = NULL;
		tmp = NULL;
	}
}

int count_for_print;
//recursive print of nodes in first-last order
void print_list_reverse(node_t *node) {
	if (node->next != NULL) {
		count_for_print++;
		print_list_reverse(node->next);
	} else {
		printf("[ ");
	}
	printf("\"%s\" ", node->factor);
	if (count_for_print == 0) {
		printf("]");
		count_for_print = 1;
	}
	count_for_print--;
}

void print_list(node_t *node) {
	printf("[ ");
	while (node != NULL) {
		printf("\"%s\" ", node->factor);
		node = node->next;
	}
	printf("]");
}

char *substring(const char* word, int x, int y) {
	int k = 0, i;
	char *sub = malloc((y-x + 1));

	for (i = x; i < y; i++) {
		sub[k++] = word[i];
	}
	sub[k] = '\0';

	return sub;
}

node_t* addFactorNode(node_t* head, char* factor){
    node_t* temp;
    temp = malloc(sizeof(node_t));
    temp->next = NULL;
    temp->factor = factor;
    if(head == NULL){
        head = temp;
    }
    else {
        temp->next = head;
        head = temp;
    }
    return head;
}

node_t* insertBottom(node_t *head, char* factor) {
  node_t* current_node = head;
  node_t* new_node;
     while ( current_node != NULL && current_node->next != NULL) {
       current_node = current_node->next;
      }

  new_node = malloc(sizeof(node_t));
  new_node->factor = factor;
  new_node->next= NULL;
  if (current_node != NULL)
    current_node->next = new_node;
  else
     head = new_node;
return head;
}

int lenght_list(node_t *node){
	int len = 0;
	while(node !=NULL){
		node = node->next;
		len++;
	}
	return len;
}
//___________________________________________________
//Array contains a position List
//the head is the end of array.
typedef struct array_int{
	int* ints;
	int len;
}array_int_t;
#define ARRAY_INT_SIZE 5

// size_array specif size of array (can be 0)
array_int_t* init_array_int(int size_array){
	int remainder = size_array % ARRAY_INT_SIZE;
	int div = size_array / ARRAY_INT_SIZE;
	int len;
	if(remainder == 0 && size_array != 0)
		len = div * ARRAY_INT_SIZE;
	else
		len = (div + 1) * ARRAY_INT_SIZE;

	array_int_t* array_int = malloc(sizeof(array_int_t));
	array_int->ints = malloc(sizeof(int) * len);
	array_int->len = 0;
	return array_int;
}

//Add item at end of array
array_int_t* add_int_array(array_int_t* array_int, int item){
	int* temp_array_int;
	if(array_int->len!=0){
		int remainder = array_int->len % ARRAY_INT_SIZE;

		if(remainder == 0){
			int size = array_int->len / ARRAY_INT_SIZE;
			int newLen = (size + 1)*ARRAY_INT_SIZE;
			temp_array_int = realloc(array_int->ints, sizeof(int)*newLen);

			if(temp_array_int == NULL){
				temp_array_int = malloc(sizeof(int)*size+1);

				for(int i = 0; i < array_int->len;i++){
					temp_array_int[i] = array_int->ints[i];
				}
			}
			array_int->ints = temp_array_int;
		}
	}
	array_int->ints[array_int->len] = item;
	array_int->len++;

	return array_int;
}

//position of int in array_int if int is in the array, otherwise negative
int contains_int(array_int_t* array_int, int pos){
	int len = array_int->len;
	for(int i = 0;i<len;i++){
		//printf("i: %d\n",i);
		if(array_int->ints[i] == pos)
			return i;
	}
	return -1;
}


// concat the array_int add_2 to array_int add_1 and return a new array_int
// if they are array_int with head at end of array, end of add_2 will be the new head of dest
array_int_t* concat_new_array_int(array_int_t* add_1, array_int_t* add_2){

	int len_add1 = add_1->len, len_add2= add_2->len;
	array_int_t* dest = init_array_int(len_add1 + len_add2);
	dest->len = len_add1 + len_add2;

	for(int i = 0;i < len_add1; i++){
		dest->ints[i] = add_1->ints[i];
	}
	for(int i = len_add1; i < dest->len; i++){
		dest->ints[i] = add_2->ints[i-len_add1];
	}
	return dest;
}

void print_array_int(array_int_t* array_int){
	for(int i = 0; i < array_int->len; i++){
		printf("%d, ", array_int->ints[i]);
	}
}


//___________________________________________________
//Struct for positions lists
typedef struct positions_lists{
	char** suffix_array;
	array_int_t** positions_array;
	int len;
}positions_lists_t;

positions_lists_t* initPositionsLists(){
	positions_lists_t *plist = malloc(sizeof(positions_lists_t));
	plist->suffix_array = NULL;
	plist->positions_array = NULL;
	plist->len=0;
	return plist;
}

// position to be inserted if not contained(0,1,...,N), otherwise negative
// if is contained will be negative
int contains_suffix(positions_lists_t* plist, char* suffix){
	if(plist->len == 0)
		return 0;
	int high = plist->len-1;
	int low = 0;
	int mid = 0;
	//printf("%s: len:%d high:%d low:%d\n",suffix, plist->len, high, low);
	while(low <= high){
		mid = (low + high)/2;
		//printf("mid:%d\n",mid);
		int cmp = strcmp(suffix,plist->suffix_array[mid]);
		if(cmp == 0)
			return -1;
		if(cmp > 0)
			low = mid +1;
		else
			high = mid -1;
		//printf("%s!: len:%d high:%d low:%d mid:%d\n",suffix, plist->len, high, low,mid);
	}
	if(strcmp(suffix,plist->suffix_array[mid])>0){
		//printf("%s va messo in pos+1: %d\n",suffix, mid+1);
		return mid+1;
	}
	else{
		//printf("%s va messo in pos: %d\n",suffix,mid);
		return mid;
	}
}

// add suffix in position_array of the postions_lists
positions_lists_t* add_suffix(positions_lists_t* plist, char* suffix){
	int pos = contains_suffix(plist,suffix);
	if(pos<0){
		return NULL;
	}
	//printf("inserire\n");
	int newLen = ++(plist->len);
	int shift = 0;
	char** temp_suffix_array= realloc(plist->suffix_array ,sizeof(char*) * newLen);
	array_int_t** temp_positions_array= realloc(plist->positions_array,sizeof(array_int_t*) * newLen);

	//if it is NULL copy the array to a new part of memory, otherwise only the part to be shifted must be modified
	if(temp_suffix_array == NULL || temp_positions_array == NULL){
		temp_suffix_array = malloc(sizeof(char*)*newLen);
		temp_positions_array = malloc(sizeof(array_int_t*) * newLen);
		for(int i = 0; i < newLen;i++){
			//printf("%s i:%d: ",suffix,i);
			if(i == pos){
				//printf("set shift\n");
				temp_suffix_array[i] = suffix;
				temp_positions_array[i] = init_array_int(0);
				shift = 1;
			}
			else if(!shift){
				//printf("senza shift\n");
				temp_suffix_array[i] = plist->suffix_array[i];
				temp_positions_array[i] = plist->positions_array[i];
			}
			else{
				//printf("con shift\n");
				temp_suffix_array[i] = plist->suffix_array[i-1];
				temp_positions_array[i] = plist->positions_array[i-1];
			}
		}
	}
	else{
		for(int i = newLen-1; i > pos;i--){
			temp_suffix_array[i] = temp_suffix_array[i-1];
			temp_positions_array[i] = temp_positions_array[i-1];
		}
		temp_suffix_array[pos] = suffix;
		temp_positions_array[pos] = init_array_int(0);
	}
	
	plist->suffix_array = temp_suffix_array;
	plist->positions_array = temp_positions_array;
	return plist;
}

// null if suffix doesn't exist
// Inserire controlo sull'elemento da inserire (duplicati)
positions_lists_t*  add_position(positions_lists_t* plist, char* suffix, int pos){
	//find suffix in the list, if exists add pos in the its positions_array
	for(int i = 0; i < plist->len; i++){
		//printf("AddPosition: Start\n");
		int cmp = strcmp(suffix,plist->suffix_array[i]);
		
		//suffix exists
		if(cmp==0){
			if(contains_int(plist->positions_array[i], pos) < 0){
				add_int_array(plist->positions_array[i],pos);
				return plist;
			}
			return NULL;
		}

		//the suffix_array is ordered so the suffix can't be after the first greater
		else if(cmp<0){
			return NULL;
		}
	}
	return NULL;
}

void printPositionsLists(positions_lists_t* plist){
	printf("{");
	for(int i = 0; i < plist->len; i++){
		printf("\'%s\': ",plist->suffix_array[i]);
		printf("[");

		//The array's head is last position
		for(int j = plist->positions_array[i]->len-1; j >= 0; j--){
			array_int_t* posArray= plist->positions_array[i];
			if (j>0){
				printf("%d, ",posArray->ints[j]);
			}
			else{
				printf("%d",posArray->ints[j]);
			}
		}
		if(i+1<plist->len)
			printf("], ");
		else
			printf("]");
	}
	printf("}\n");
}

array_int_t* get_positions_array(positions_lists_t* posList,char* suffix){
	int len = posList->len;
	for(int i = 0; i < len; i++){
		if(strcmp(suffix,posList->suffix_array[i])== 0 ){
			return posList->positions_array[i];
		}
	}
	return NULL;
}

//___________________________________________________
//Struct Array of Strings
typedef struct array_strings{
	char** strings;
	int len;
}array_strings_t;

array_strings_t* initArray_strings(){
	array_strings_t* array_strings = malloc(sizeof(array_strings_t));
	array_strings->strings = NULL;
	array_strings->len = 0;
	return array_strings;
}

array_strings_t* addString(array_strings_t* array_strings, char* string){
	int new_len = ++(array_strings->len);
	char** temp_strings = realloc(array_strings->strings, sizeof(char*) * new_len);

	if(temp_strings == NULL){
		printf("addString: realloc fallito!\n");
		temp_strings = malloc(sizeof(char*)*new_len);
		for(int i =0; i < new_len - 1;i++){
			temp_strings[i] = array_strings->strings[i];
		}
	}

	array_strings->strings = temp_strings;
	array_strings->strings[new_len-1] = string;
	return array_strings;
}

void print_array_strings(array_strings_t* array_strings){
	printf("[");
	int len = array_strings->len;
	for(int i = 0; i < len-1; i++){
		printf("%s, ",array_strings->strings[i]);
	}
	if(len==0)
		printf("]");
	else
		printf("%s]", array_strings->strings[len-1]);
}

void clear_array_strings(array_strings_t* array_strings){
	array_strings->strings=NULL;
	array_strings->len = 0;
}



/***** Placeholder struct for array NON UTILIZZATO NEL CODICE******/ 
typedef struct {
  int *array;
  size_t used;
  size_t size;
} Array;

void initArray(Array *a, size_t initialSize) {
  a->array = malloc(initialSize * sizeof(int));
  a->used = 0;
  a->size = initialSize;
}

void insertArray(Array *a, int element) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
  if (a->used == a->size) {
    a->size *= 2;
    a->array = realloc(a->array, a->size * sizeof(int));
  }
  a->array[a->used++] = element;
}

void freeArray(Array *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}
