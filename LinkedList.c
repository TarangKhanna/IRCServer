
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LinkedList.h"

//
// Initialize a linked list
//
void llist_init(LinkedList * list)
{
	list->head = NULL;
}

//
// It prints the elements in the list in the form:
// 4, 6, 2, 3, 8,7
//
void llist_print(LinkedList * list) {
	
	ListNode * e;

	if (list->head == NULL) {
		printf("{EMPTY}\n");
		return;
	}

	printf("{");

	e = list->head;
	while (e != NULL) {
		printf("%d", e->value);
		e = e->next;
		if (e!=NULL) {
			printf(", ");
		}
	}
	printf("}\n");
}

//
// Appends a new node with this value at the beginning of the list
//
void llist_add(LinkedList * list, int value) {
	// Create new node
	ListNode * n = (ListNode *) malloc(sizeof(ListNode));
	n->value = value;
	
	// Add at the beginning of the list
	n->next = list->head;
	list->head = n;
}

//
// Returns true if the value exists in the list.
//
int llist_exists(LinkedList * list, int value) {
    
    ListNode * e;

	if (list->head == NULL) {
		printf("{EMPTY}\n");
		return 0;
	}

	e = list->head;
	while (e != NULL) {
		if(value == (e->value)) {
          return 1;
        }
		e = e->next;
		if (e==NULL) {
			return 0;
		}
	}
	return 0;
}

//
// It removes the entry with that value in the list.
//
int llist_remove(LinkedList * list, int value) {
    ListNode * e;

	if (list->head == NULL) {
		printf("{EMPTY}\n");
		return 0;
	}

	e = list->head;
	while (e != NULL) {
        if((e->next) != NULL) {
		 if(value == (e->next->value)) {
            e->next = e->next->next; // remove
            return 1;
         } 
       }
		e = e->next;
	}
   
	return 0;
}

//
// It stores in *value the value that correspond to the ith entry.
// It returns 1 if success or 0 if there is no ith entry.
//
int llist_get_ith(LinkedList * list, int ith, int * value) {
    int counter = 0; // use counter 
    ListNode * e;

	if (list->head == NULL) {
		printf("{EMPTY}\n");
		return 0; 
	}

	e = list->head;
	while (e != NULL) {
       
		if(counter == ith) {
          *value = e->value;
          return 1;
        }
        counter++; 
		e = e->next;
		if (e==NULL) {
			return 0;
		}
	}
	return 0;
}

//
// It removes the ith entry from the list.
// It returns 1 if success or 0 if there is no ith entry.
//
int llist_remove_ith(LinkedList * list, int ith) { // 6 

    int counter = 0; // use counter 
    ListNode * e;

	if (list->head == NULL) {
		printf("{EMPTY}\n");
		return 0; 
	}

	e = list->head;
	while (e != NULL) {
       
		if(counter == (ith-1)) {
          e->next = e->next->next;
          return 1;
        }
        counter++; 
		e = e->next;
		if (e==NULL) {
			return 0;
		}
	}
	return 0;
}

//
// It returns the number of elements in the list.
//
int llist_number_elements(LinkedList * list) {
    int counter = 0; // use counter 
    ListNode * e;

	if (list->head == NULL) {
		printf("{EMPTY}\n");
		return 0; 
	}

	e = list->head;
	while (e != NULL) {
        counter++; 
		e = e->next;
		if (e==NULL) {
			return counter;
		}
	}
	return counter;
}


//
// It saves the list in a file called file_name. The format of the
// file is as follows:
//
// value1\n
// value2\n
// ...
//
int llist_save(LinkedList * list, char * file_name) {
    FILE *fp = fopen(file_name, "ab+");
    ListNode * e;
    e = list->head;
    if(fp == NULL) {
       return 0;
    } else {	  
	  while (e != NULL) {
          fprintf(fp,"%d\n", e->value);
		  e = e->next;
	  }  
    }
    fclose(fp);
	return 0;
}

//
// It reads the list from the file_name indicated. If the list already has entries, 
// it will clear the entries.
//
int llist_read(LinkedList * list, char * file_name) { 
    FILE *fp = fopen(file_name, "r");
    ListNode * e;
    e = list->head;
    int read;
    if(e != NULL) {
         llist_clear(list);
    }
    while(!feof(fp)) {  
          fscanf(fp,"%d\n", &read);
          llist_add(list, read);
          //e = e->next;
   }  
    fclose(fp);
	return 1;
}


//
// It sorts the list. The parameter ascending determines if the
// order si ascending (1) or descending(0).
//
void llist_sort(LinkedList * list, int ascending) {
    ListNode *e;
    ListNode *lptr;
    
    int temp;
   
    e = list->head;
    lptr = e->next;
    if(e == NULL) {
    
    }  
    else if(ascending == 1) {
      while(e != NULL) {
         while ((lptr) != NULL)
         {
             if ((e->value) > (lptr->value))
             { 
                 temp = e->value;
                 e->value = lptr->value;
                 lptr->value = temp;
                
             }
             lptr = lptr->next;
           }
           e = e->next;
           lptr = e;
      }
     
    } else if(ascending == 0) {
 
         while(e != NULL) {
          while ((lptr) != NULL) {
             if (e->value < lptr->value)
             { 
                 temp = e->value;
                 e->value = lptr->value;
                 lptr->value = temp;
             }
             lptr = lptr->next;
           }
           e = e->next;
           lptr = e;
      }
     
    }
  
}

//
// It removes the first entry in the list and puts value in *value.
// It also frees memory allocated for the node
//
int llist_remove_first(LinkedList * list, int * value) {
    ListNode * e;
    //ListNode * e2;
       e = list->head; 
	   if (e != NULL) { 
	   	 *value = e->value; 
          list->head = e->next;; 
          free(e);
   	      return 1;
	   }   
      return 0;
}

//
// It removes the last entry in the list and puts value in *value/
// It also frees memory allocated for node.
//
int llist_remove_last(LinkedList * list, int *value) {
    ListNode * e;

	if (list->head == NULL) {
		return 0;
	}   

	e = list->head; 
   
	while ((e->next->next) != NULL) { 
         e = e->next;
         
	}
    *value = e->next->value;
    free(e->next);
    e->next = NULL;
   
	return 1;
}

//
// Insert a value at the beginning of the list.
// There is no check if the value exists. The entry is added
// at the beginning of the list.
//
void llist_insert_first(LinkedList * list, int value) {
      ListNode * e;
      ListNode * n2;

      n2 = (ListNode*) malloc(sizeof(ListNode));
      n2->value = value; 
      e = list->head; 
	   if (e == NULL) { 
	   	  e = n2;
          n2->next = NULL;
       } else {
          list->head = n2;; 
          n2->next = e;
   	     
	   }   
  
}

//
// Insert a value at the end of the list.
// There is no check if the name already exists. The entry is added
// at the end of the list.
//
void llist_insert_last(LinkedList * list, int value) {
     ListNode *e;
     ListNode *n2;
     n2 = (ListNode*) malloc(sizeof(ListNode));
     n2->value = value;
     e = list->head;
     if(e == NULL) {
        list->head = n2;
        list->head->next = NULL;
     } else {  
        while(e->next != NULL) {
            e = e->next;
        }
        e->next = n2;
        n2->next = NULL;
     }
}

//
// Clear all elements in the list and free the nodes
//
void llist_clear(LinkedList *list)
{
   ListNode *e;
   e = list->head;
   while(e != NULL) {
     e = e->next;
     if(e == NULL) {
        list->head = NULL;
        break;
     }
     free(e);
     
   }
 
}
