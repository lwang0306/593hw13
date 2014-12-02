#include <stdio.h>
#include "lc4_memory.h"

struct row_of_memory* add_to_list    (struct row_of_memory* head, 
				      short unsigned int address, 
				      short unsigned int contents) 
{
	/* takes in head pointer to a linked list: head
	   creates a new node with address & contents passed in
	   adds node to end of linked list pointed to by head
	   if list is empty, new node is now head
	   always returns head pointer */
	   
	/* add node to the end of linked list pointed to by "head"
	   if list doesn't exist, create 1st node */
	 

	/* allocate memory for a single node */

	/* populate fields in newly allocated node w/ address&contents */

	/* if head==NULL, node created is the new head of the list! */

	/* otherwise, traverse linked list until we reach the end */

	/* add node to the end of list */

	/* always return pointer to head of linked list */

	return head ;   
}

struct row_of_memory* search_address (struct row_of_memory* head, 
				      short unsigned int address ) 
{
	/* traverse linked list, searching each node for "address"  */

	/* return pointer to node in list that matches */

	/* return NULL if list is empty or if "address" isn't found */
	 
	return NULL ;
}

struct row_of_memory* search_opcode  (struct row_of_memory* head, 
				      short unsigned int opcode  ) 
{
	/* traverse linked list until node is found with matching opcode
	   AND "assembly" field of node is empty */

	/* return pointer to node in list that matches */

	/* return NULL if list is empty or if no matching nodes */
	 
	return NULL ;
}

void print_list (struct row_of_memory* head ) 
{
	/* make sure head isn't NULL */

	/* print out a header */

	/* traverse linked list, print contents of each node */	

	return ;
}

struct row_of_memory* delete_list    (struct row_of_memory* head ) 
{
	/* delete entire list node by node */

	return NULL ;
}
