typedef struct Node {
  void * data;
  struct Node* next; 
  struct Node* prev;
}Node;

typedef struct LIST{
  int size; 
  struct Node *curr; 
  struct LIST *next;  
  struct Node *prev;
  struct Node *first;
  struct Node *last;
  struct Node *available; 
  
}LIST;


LIST *ListCreate();

int ListAdd(LIST *list, void *item);

int ListCount( LIST *list);

void *ListCurr( LIST *list);

void *ListNext( LIST *list);

void *ListFirst( LIST *list);

void *ListLast( LIST *list);

void *ListPrev(LIST *list);

int ListInsert( LIST *list, void *item);

int ListAppend(LIST *list, void *item);

int ListPrepend( LIST *list, void *item);

void *ListRemove( LIST *list);

void ListConcat( LIST *list1,  LIST *list2);

void *ListTrim( LIST *list);

void *ListSearch(LIST *list,int(*compare)(), void *comparisonArg);

void  ListFree( LIST *list,  void (*itemFree)());