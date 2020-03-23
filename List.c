#include <stdio.h>
#include <stdlib.h>
#include "List.h"
//#include <iostream>
#define MAX_STATES 100
#define MAX_HEAD 100




static LIST listOfNodes[MAX_STATES];
LIST *headAvailable;


static Node nodes[MAX_STATES];
Node *availableNode; 
int head = 0;
int found = 0;
int node = 0; 
int headtally = 0;
int end = 0; 
int before = 0;  
int nodeTally = 0; 


//creates a list that initially initializes everything in the list of nodes and nodes array. List of nodes keeps track of how many heads are creates and lsit of nodes keeps track of how many ndoes have been taken
LIST *ListCreate(){

//if nodes have not been initialized
  if(node == 0){
  for(int i = 0; i <MAX_STATES; i++){
     nodes[i].data = NULL; 
     nodes[i].next = NULL;
     nodes[i].prev = NULL;
    if(i > 0){
          nodes[i-1].next = &nodes[i];
          //listOfNodes[i-1].next = &listOfNodes[i];
        }

       }
    node = 1;
    availableNode = &nodes[0];

  }
  //if head has not been initialized
   if(head == 0){
       for(int i = 0; i <MAX_HEAD; i++){
          //nodes[i].data = NULL; 
          //nodes[i].next = NULL;
          listOfNodes[i].next = NULL;
          if(i > 0){
            //nodes[i-1].next = &nodes[i];
            listOfNodes[i-1].next = &listOfNodes[i];
          }
        }
        head =1;
        headAvailable = &listOfNodes[0];
        //headAvailable->size = headAvailable->size + 1;
   }

   if(headtally > MAX_HEAD){
     return NULL;
   }

  //keeps track of how many heads have been created
   //initialize new list that is being created
   headtally++;
   LIST *newarray = headAvailable;
   headAvailable = headAvailable->next;
   newarray->size = 0; 
   newarray->last = NULL;
   newarray->first = NULL;
   newarray->curr= NULL;

   return newarray;
}

//list add adds an item to a list

int ListAdd(LIST *list, void *item){

  Node *nextAvailable = availableNode;
  if(nodeTally > MAX_STATES){
    return -1; 
  }

  if(availableNode){
    //if list size is 0 
     if(list->size == 0){
      availableNode = availableNode -> next;
      nextAvailable->data = item;
      nextAvailable->next = NULL; 
      nextAvailable->prev = NULL;

      list->first = nextAvailable;
      list->last = nextAvailable;
      list->curr = nextAvailable;
      list->size++;
      nodeTally++;
      return 0;
      //list->size++;
    }/*else if (list->size > 0 && list->curr == list->first){ //if list size is greater than one but the current pointer is at the first item 
      availableNode = availableNode->next;
      
      nextAvailable->data = item;
      nextAvailable->prev = list->curr; 
      nextAvailable->next = list->curr->next;
      list->first->next = nextAvailable;
     
      //nextAvailable->next = list->first;
      //list->first->prev = nextAvailable;
      //nextAvailable->prev = NULL;
      //list->first = nextAvailable;
      list->curr = nextAvailable;
      list->size++;
      nodeTally++;
      return 0;
    }*/else if (list->size > 0 && list->curr == list->last){ //if the current pointer is at the end of the list 
      availableNode = availableNode->next;

      nextAvailable->data = item;
      nextAvailable-> next = NULL;
      nextAvailable->prev = list->last;

      list->last->next = nextAvailable;
      list->last = nextAvailable;
      list->curr = nextAvailable; 
      list->size++;
      nodeTally++;
      return 0;

    }else if(list->size >0 && before == 1){ //if the current pointer points to before the list
      availableNode = availableNode->next;
      
      nextAvailable->data = item;
      nextAvailable->next = list->first;
      list->first->prev = nextAvailable;
      nextAvailable->prev = NULL;

      list->first = nextAvailable;
      list->curr = nextAvailable;
      list->size++;
      nodeTally++;
      before = 0; 
      return 0;
    }else if(list->size > 0 && end == 1){//if the current pointer points to beyond the list
      availableNode = availableNode->next;
      nextAvailable->data = item;
      nextAvailable-> next = NULL;
      nextAvailable->prev = list->last;
      list->last->next = nextAvailable;

      list->last = nextAvailable;
      list->curr = nextAvailable; 
      list->size++;
      nodeTally++;
      end = 0; 
      return 0; 
    }else{
      availableNode = availableNode->next;
      
      nextAvailable->data = item;
      nextAvailable->prev = list->curr;
      nextAvailable->next = list->curr->next;
      //list->last = nextAvailable;
      /*if(list->curr == list->last) { //if the current node is the last node and it skipped the boundary case above then we make the current node the last node
        list->last = nextAvailable;
      }*/ 
      list->curr->next->prev = nextAvailable;
      list->curr->next = nextAvailable; 
      list->curr = nextAvailable;
      list->size++;
      nodeTally++;
      return 0;
    }
    }

    return -1;
}

//returns list size
int ListCount( LIST *list){
  return list->size; 
} 

//returns current pointer of the list
void *ListCurr(LIST *list){
  //printf("%4d", *(int *)list->curr->data);
  if(list->curr == NULL){ //check if list->curr is null because this would indicate that the list is beyond or before 
    return NULL;
  }else if (list->size == 0){ //this is to check if the list has never been used
    list->curr = NULL;
    return list->curr;
  }else if(before == 1){
    list->curr = NULL;
    return list->curr;
  }else if(end == 1){
    list->curr = NULL; 
    return list->curr; 
  }
  return list->curr->data;
}


//moves the pointer to the next item in the list 
void *ListNext( LIST *list) {
  if(list->curr == list->last){ //if we are at the end of the lsit then we need the pointer to move beyond it and set our flag of end to indicate we are beyond the list
    list->curr = list->curr->next;
    end = 1; 
    return list->curr;
  }/*else if(list->curr == list->first && before != 1){ // if the current pointer is first and not before the beginng of the list
    list->curr = list->curr->next;
    return list->curr->data;
  }else if(list->curr == NULL && before == 1){//if the current pointer is before the beginning of the list then we make the pointer the first item in the lsit
    list->curr = list->first;
    before = 0; 
    return list->curr->data; 
  }else if (list->size == 0){//if the list is empty then the current pointer moves beyond the beginnign of the list
    end = 1; 
    list->curr = NULL;
    return list->curr;
  }*/else if (end == 1){// if the pointe ris beyond the end of the list the pointer cant be updated. 
    end = 1; 
    list->curr = NULL;
    return list->curr; 
  }
  else if (before == 1){
    list->curr = list->first; 
    before = 0; 
    return list->curr->data; 
  }
    else{
    list->curr = list->curr->next;
   // printf("%4d", *(int *)list->curr->data);
    return list->curr->data;
  }
}


//returns a pointer to the beginning of the list 
void *ListFirst( LIST *list) {
 
  if(list->size == 0){ //is we have never used the lsit then we dont have a first yet
    list->first = NULL;
    return list->first;
  }else{
  before = 0; 
  end = 0;
  list->curr = list->first;
  return list->curr->data;
  }
  

}

//returns a pointer to the end of the list

void *ListLast( LIST *list){
  if(list->size == 0){  //if we have never used the list then we dont have a last yet
      list->last = NULL;
      return list->last;
    }
  if(end == 1){//if the pointer is current at the end then we need to turn the flag off for beyond the end of the list 
    end = 0; 
  }
  else{

    before = 0; 
    end = 0;
    list->curr = list->last; 
  }
  return list->curr->data;
}

//moves the pointer to the previous item in the list

void *ListPrev(LIST *list){
  if(list->curr == list->first){ //if the pointer is the first then the pointer before is beyond the beginning and so the flag for beyond needs to be turned on
    list->curr = list->curr->prev;
    before = 1; 
    return list->curr;
  }else if(before == 1){ //if it is already beyond then we cant do anything
    list->curr = NULL;
    return list->curr; 
  } else if (list->size == 0){ //if the list size is 0 then we have to move the pointer to beyond the end of the list
    before = 1; 
    list->curr = NULL; 
    return list->curr; 
    }else if(list->curr == list->last && end != 1){ // if the current pointer is at the end then we have to move the pointer to beyond the end of the list 
    list->curr = list->last->prev;
    return list->curr->data;
  }else if(list->curr == NULL && end == 1){ //if we are at beyond the end of the list then we have to go back to the actual last of the lsit 
    list->curr = list->last;
    end = 0;
    return list->curr->data;
  }
  else{
    list->curr = list->curr->prev;
    //printf("%4d", *(int *)list->curr->data);
    return list->curr->data;
  }
  
  
}

//inserts at an item before the current pointer
int ListInsert( LIST *list, void *item){
if(nodeTally > MAX_STATES){ //check if node tally is available 
    return -1; 
  }
     Node *nextAvailable = availableNode;
     //check if the current pointer is before thenwe make the first of the lsit the new node
    if(before == 1){
      availableNode = availableNode->next;
      
      nextAvailable->data = item;
      nextAvailable->next = list->first;
      list->first->prev = nextAvailable;
      nextAvailable->prev = NULL;

      list->first = nextAvailable;
      list->last = nextAvailable;
      list->curr = nextAvailable;
      list->size++;
      nodeTally++;
      before = 0; 
      return 0;
    }else if(end == 1){//if beyond the end then we need the new node to be the last
       availableNode = availableNode->next;
      nextAvailable->data = item;
      nextAvailable-> next = NULL;
      nextAvailable->prev = list->last;
      list->last->next = nextAvailable;
      list->last = nextAvailable;
      list->curr = nextAvailable; 
      list->size++;
      nodeTally++;
      end = 0; 
      return 0;
    }else if(list->size == 0){ //if size is 0 we can create a new node 
      availableNode = availableNode -> next;
      nextAvailable->data = item;
      nextAvailable->next = NULL; 
      nextAvailable->prev = NULL;
      list->first = nextAvailable;
      list->last = nextAvailable;
      list->curr = nextAvailable;
      list->size++;
      nodeTally++;
      return 0;
      }else{
    ListPrev(list);//move pointer back and ListAdd where the rest of the boundary cases are covered. 
    //availableNode = list->curr;
    return ListAdd(list, item);
    }
    return -1;
}

//adds a item to the end of the list 

int ListAppend(LIST *list, void *item){
  if(nodeTally >MAX_STATES){//check if there are any available nodes 
    return -1; 
  }
  //list->curr = list->last->prev; 
  //ListLast(list); 

   Node *nextAvailable = availableNode;
   //check if the list size is 0 
  if (list->size == 0){
      availableNode = availableNode -> next;
      nextAvailable->data = item;
      nextAvailable->next = NULL; 
      nextAvailable->prev = NULL;

      list->first = nextAvailable;
      list->last = nextAvailable;
      list->curr = nextAvailable;
      list->size++;
      nodeTally++;
      return 0;
      }else if (list->size > 0 && before == 1){

      availableNode = availableNode -> next;

      nextAvailable->data = item;
      nextAvailable->next = NULL; 
      nextAvailable->prev = list->last;
      list->last->next = nextAvailable;

      //list->first = nextAvailable;
      list->last = nextAvailable;
      list->curr = nextAvailable;
      before = 0; 
      list->size++;
      nodeTally++;
      return 0;
    }else if(list->size >0 && end == 1){

      availableNode = availableNode -> next;

      nextAvailable->data = item;
      nextAvailable->next = NULL; 
      nextAvailable->prev = list->last;
      list->last->next = nextAvailable;

      //list->first = nextAvailable;
      list->last = nextAvailable;
      list->curr = nextAvailable;
      end = 0; 
      list->size++;
      nodeTally++;
      return 0;
    }else{
      availableNode = availableNode -> next;

      nextAvailable->data = item;
      nextAvailable->next = NULL; 
      nextAvailable->prev = list->last;
      list->last->next = nextAvailable;

      //list->first = nextAvailable;
      list->last = nextAvailable;
      list->curr = nextAvailable; 
      list->size++;
      nodeTally++;
      return 0;
    } 

}

//adds the item to the beginning of the list 

int ListPrepend(LIST *list, void *item){
  if(nodeTally > MAX_STATES){
    return -1; 
  }
    ListFirst(list);
  
   Node *nextAvailable = availableNode;
   //if the list size is 1 then we need it to be at the beginning of the list 
  if (list->size == 1){
       availableNode = availableNode->next;
      
      nextAvailable->data = item;
      nextAvailable->next = list->first;
      list->first->prev = nextAvailable;
      nextAvailable->prev = NULL;

      list->first = nextAvailable;
      list->curr = nextAvailable;
      list->size++;
      nodeTally++;
      return 0;


      }else if (list->size >1 && list->curr == list->first){ //if the current is the first then we need to add before it 
      availableNode = availableNode->next;
      
      nextAvailable->data = item;
      nextAvailable->next = list->first;
      list->first->prev = nextAvailable;
      nextAvailable->prev = NULL;

      list->first = nextAvailable;
      list->curr = nextAvailable;
      list->size++;
      nodeTally++;
      return 0;

    }else{
      return ListAdd(list, item); //send list to lsit add because the rest of the boundary cases are covered there
    }
 return -1; 
}


//remove the current item from the list 

void *ListRemove(LIST *list){
  Node *temp = list->curr;

  

  /*if(list->curr == list->first && list->size != 1){ //if the curent item is the first one and there are more items in the list 
     Node *returnNode = list->first;
      Node *rep = list->curr;
       list->first = list->first->next;
       list->curr = list->first;
       list->size = list->size-1;
      if(nodeTally == MAX_STATES){//return the node to available and decrement the amount of ndoes that have been used
        returnNode->next = NULL;
        availableNode = returnNode;
        nodeTally--;
      }else{
        returnNode->next = availableNode;
        availableNode->prev = returnNode;
        nodeTally--;
      }
      return rep->data;

  }*/
  if(list->size == 0){ //if the size is 0 then we can remove anything 
    return NULL;

  }else if( end == 1 || before == 1){
    return NULL; 
  }else if(list->size == 1 && list->curr){//if the size is 1 then we have a new empty list 
     Node *returnNode = list->curr; 
     Node *rep = list->curr; 
    list->first = NULL;
    list->curr = NULL;
    list->last = NULL;
    list->size--;
    returnNode->data = NULL;
    returnNode ->prev = NULL; 

    if(nodeTally == MAX_STATES-1){// check if the nodes have all been maxed out because this indicates that there is no next for the current node to point to in our list of available nodes
      returnNode->next = NULL;
      availableNode = returnNode;
      nodeTally--;
    }else{
      returnNode->next = availableNode;
      availableNode = returnNode;
      nodeTally--;
    }
    return rep->data; 
  }else{
     Node *returnNode = list->curr; 
     Node *rep = list->curr; 

     if(list->curr == list->last){
      list->last = list->curr->prev; 
      list->last->next = NULL;
      list->curr = NULL;
      end =1; 
      list->size--;
     }else if (list->curr == list->first){
      list->first = list->curr->next; 
      list->curr = list->first;
      list->curr->prev = NULL;
      list->size--;
     }else{
      list->curr->prev->next = list->curr->next; 
      list->curr->next->prev = list->curr->prev; 

      Node *prevNode = list->curr->prev; 

      list->curr = list->curr->next;
      list->curr->prev = NULL;
      list->curr->prev = prevNode;
      list->size--; 
     }

    returnNode->data = NULL;
    returnNode ->prev = NULL; 

    if(nodeTally == MAX_STATES-1){// check if the nodes have all been maxed out because this indicates that there is no next for the current node to point to in our list of available nodes
      returnNode->next = NULL;
      availableNode = returnNode;
      nodeTally--;
    }else{
      returnNode->next = availableNode;
      availableNode = returnNode;
      nodeTally--;
    }

    return rep->data; 
  }
  /*else if(list->curr == list->last && list->size != 1){//if the current is the last then we need to remove the last item and update the last pointer
     Node *returnNode = list->curr; 
     Node *rep = list->curr; 
    list->last = list->curr->prev; 
    list->curr = list->last;
    list->curr->next = NULL;
    list->size--;

    if(nodeTally == MAX_STATES){ // check if the nodes have all been maxed out because this indicates that there is no next for the current node to point to in our list of available nodes
      returnNode->next = NULL;
      availableNode = returnNode;
      nodeTally--;
    }else{
      returnNode->next = availableNode;
      availableNode->prev = returnNode;
      nodeTally--;
    }
    return rep->data;
  }else if(before == 1 || end == 1){
    return NULL; 
  }
  else{
   Node *ava = list->curr;
   Node *item = list->curr;
   Node *prevNode = list->curr->prev;
   Node *nextNode = list->curr->next;
  
  prevNode->next = nextNode;
  nextNode->prev = prevNode;
  list->curr = nextNode;
  //ava->next = NULL;
  ava->prev = NULL;
  //availableNode = ava;
  list->size = list->size-1;

  if(nodeTally == MAX_STATES){// check if the nodes have all been maxed out because this indicates that there is no next for the current node to point to in our list of available nodes
      ava ->next = NULL;
      availableNode = ava;
      nodeTally--;
    }else{
      ava ->next = availableNode;
      availableNode->prev = ava ;
      nodeTally--;
    }
  return  item->data;
  }*/

}

//combining two lists together 
void ListConcat( LIST *list1,  LIST *list2){
    if(list1->last != NULL && list2->first != NULL){//make sur ethey are both not empty
        list1->last->next = list2->first;
        list2->first->prev = list1->last; 
        list1->last = list2->last;
        list1->size = list1->size +list2->size;
        //list2 = NULL;
        list2->first = NULL;
        list2->last = NULL;
        list2->curr = NULL;
        //headAvailable = list2;
         LIST *availableHead = list2;
        if (availableHead) {
          availableHead->next = headAvailable;
          headAvailable = availableHead;
        }
        headtally--;
    }else if (list2->first != NULL && list1->size == 0){ //if the first list is empty then make the first lsit the second lsit 
      list1->first = list2->first; 
      list1->last = list2->last; 
      list1->curr = NULL;
      before = 1; 
      list1->size = list2->size; 
      //list2 = NULL;
      list2->first = NULL;
      list2->last = NULL;
      list2->curr = NULL;
       LIST *availableHead = list2;
      if (availableHead) {
          availableHead->next = headAvailable;
          headAvailable = availableHead;
        }
      headtally--;
    }
    
    //int l = list1->size + list2->size;
}


//remove the last item of the list 

void *ListTrim(LIST *list) {
  if(list->size == 1 ){//if the only item then we need to create a new empty array 
     Node *returnNode = list->last; 
     Node *rep = list->last; 

    list->first = NULL;
    list->curr = NULL;
    list->last = NULL;
    list->size--;
    availableNode->data = NULL;
    availableNode ->prev = NULL; 

    if(nodeTally == MAX_STATES){//similar to remove, check if we have a full amount of nodes because used because then the pointer must be null because it is the only one available
      returnNode->next = NULL;
      availableNode = returnNode;
      nodeTally--;
    }else{
      returnNode->next = availableNode;
      availableNode->prev = returnNode;
      nodeTally--;
    }
    return rep->data; 
  }else if(list->size == 0){ //if the list size is 0 we can remove anything so it is null 
    return NULL;
    }else{
//struct Node *temp = list->last;

   Node *returnNode = list->last; 
   Node *rep = list->last; 
   list->last = list->last->prev;
   list->last->next = NULL;
   list->size = list->size-1;
  //temp = NULL;
   list->curr = list->last;
   //nodeTally--;
   if(nodeTally == MAX_STATES){//similar to remove, check if we have a full amount of nodes because used because then the pointer must be null because it is the only one available
      availableNode = returnNode;
      nodeTally--;
    }else{
      returnNode->next = availableNode;
      availableNode->prev = returnNode;
      nodeTally--;
    }

  return rep->data;
  }
  
}

//find the item that matches the comparisonArg using the routine 

void *ListSearch(LIST *list,int(*compare)(), void *comparisonArg){

    Node* current = list->curr;  // Initialize current. then set a while loop to end when we have reached the end of the list. 
    while (current != NULL) 
    { 
      Node* item = current->data;
      if((*compare)(item, comparisonArg)){
        list->curr = current;
        //printf("%4d", *(int*)list->curr->data);
        return list->curr->data;
      }
        current = current->next; 
      }
      list->curr = current; 
      end = 1; 
      return NULL;

}
    
//removes everything from a list that needs to be deleted
//same as LIstsearch -> navigate trough list until we have reached the end
void ListFree( LIST *list,  void (*itemFree)()) {
  
    ListFirst(list);   

    while(list->curr != NULL)
    {
        (*itemFree)(list->first->data);
        ListRemove(list);   //use remove to remove the items from the list 
        list->size--;
        //printf("%4d", nodeTally);
    }

  nodeTally--;
  headAvailable =list;
  headtally--;
  //list->first = NULL;
  //list->last = NULL;
  //list->next =
  


}

