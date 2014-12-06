#include <stdio.h>
#include "lc4_memory.h"
#include <string.h>
#include <stdlib.h>

struct row_of_memory* add_to_list    (struct row_of_memory* head, 
				      short unsigned int address, 
				      short unsigned int contents) 
{
	/* allocate memory for a single node */
	struct row_of_memory* node = malloc(sizeof(struct row_of_memory));
	if (node == NULL) {
		fprintf(stderr, "Error: no space on the heap.\n");
		return NULL;
	}
	/* populate fields in newly allocated node w/ address&contents */
	node->address = address;
	node->contents = contents;
	node->label = NULL;
	node->assembly = NULL;
	node->next = NULL;

	/* if head==NULL, node created is the new head of the list! */
	if (head == NULL) return node;
	/* otherwise, traverse linked list until we reach the end */
	else {
		struct row_of_memory* headcopy = head;
		while (headcopy->next != NULL) {
			headcopy = headcopy->next;
		}
		/* add node to the end of list */
		headcopy->next = node;
		/* always return pointer to head of linked list */
		return head ;
	}   
}

struct row_of_memory* search_address (struct row_of_memory* head, 
				      short unsigned int address ) 
{
	/* traverse linked list, searching each node for "address"  */
	while (head != NULL) {
		if (head->address == address) {
			/* return pointer to node in list that matches */
			return head;
		}
		head = head->next;
	}
	/* return NULL if list is empty or if "address" isn't found */	 
	return NULL ;
}

struct row_of_memory* search_opcode  (struct row_of_memory* head, 
				      short unsigned int opcode  ) 
{
	/* traverse linked list until node is found with matching opcode
	   AND "assembly" field of node is empty */
	while (head != NULL) {
		if ((head->contents)/0x1000 == opcode && 
			head->assembly == NULL)
			/* return pointer to node in list that matches */
			return head;
		head = head->next;
	}
	/* return NULL if list is empty or if no matching nodes */	 
	return NULL ;
}

void print_list (struct row_of_memory* head ) 
{
	/* make sure head isn't NULL */
	if (head == NULL) return ;

	/* print out a header */
	struct row_of_memory* headcopy = head;
	int max_len_label = 0;
	while (headcopy != NULL) {
		if (headcopy->label != NULL && 
			strlen(headcopy->label) > max_len_label)
			max_len_label = strlen(headcopy->label);
		headcopy = headcopy->next;
	}
	if (max_len_label < 7) max_len_label = 7;
	printf("%s", "<label>");
	int i = 0;
	while (i + 6 < max_len_label) {
		printf("%s", " ");
		i++;
	}
	printf("%s\n", "<address> <contents> <assembly>");

	/* traverse linked list, print contents of each node */	
	while (head != NULL) {
		int num_blank;
		if (head->label == NULL) {
			num_blank = max_len_label + 1;
		}
		else {
			num_blank = max_len_label + 1 - strlen(head->label);
			printf("%s", head->label);
		}
		while (num_blank > 0) {
			printf("%s", " ");
			num_blank--;
		}
		printf("%04x      ", head->address);
		printf("%04x       ", head->contents);
		if (head->assembly != NULL)
			printf("%s\n", head->assembly);
		else printf("\n");
		head = head->next;
	}

	return ;
}

struct row_of_memory* delete_list    (struct row_of_memory* head ) 
{
	/* delete entire list node by node */
	/* deref head to get the real head */
   struct row_of_memory** headPtr = &head;
   /*struct row_of_memory* current = head;*/
   struct row_of_memory* next = NULL;
 
   while (head != NULL) 
   {
       next = head->next;
       if (head->label != NULL) free(head->label);
       if (head->assembly != NULL) free(head->assembly);
       free(head);
       head = next;
   }
   
   /* deref head to affect the real head back in the caller. */
	
	return *headPtr;
}