#include <stdio.h>
#include "lc4_memory.h"
#include <stdlib.h>
#include <string.h>


/*----------------delete following after loader is ok---------------------------*/
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
		printf("%04X      ", head->address);
		printf("%04X       ", head->contents);
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
/*----------------delete above after loader is ok---------------------------*/





/*-------------start of disassembler ------------------------------*/
int reverse_assemble (struct row_of_memory* memory) 
{	
	struct row_of_memory* node = NULL;
	int subopcode = 0;
	int DR = 0;
	int SR1 = 0;
	int SR2 = 0;

	/* get the max length of label to allocate for assembly string*/
	struct row_of_memory* headcopy = memory;
	int max_len_label = 0;
	while (headcopy != NULL) {
		if (headcopy->label != NULL && 
			strlen(headcopy->label) > max_len_label)
			max_len_label = strlen(headcopy->label);
		headcopy = headcopy->next;
	}

	do {
		/* find opcode 0001 */
		node = search_opcode(memory, 1);	
		if (node != NULL) { /* if the opcode is 0001 */
			/* read subopcode, DR and SR1 */
			DR = (node->contents) / 512 % 8;
			SR1 = (node->contents) / 64 % 8;
			subopcode = (node->contents) / 8 % 8;
			SR2 = (node->contents) % 8;
			char* assem = malloc(17 * sizeof(char));
			/* examine subopcode */
			if (subopcode == 0) {
				sprintf(assem, "ADD R%d, R%d, R%d", DR, SR1, SR2);
			}
			else if (subopcode == 1) {
				sprintf(assem, "MUL R%d, R%d, R%d", DR, SR1, SR2);
			}
			else if (subopcode == 2) {
				sprintf(assem, "SUB R%d, R%d, R%d", DR, SR1, SR2);
			}
			else if (subopcode == 3) {
				sprintf(assem, "DIV R%d, R%d, R%d", DR, SR1, SR2);
			}
			else if (subopcode >= 4) {
				int imm5 = (node->contents) % 32;
				if (imm5 > 15) {
					imm5 = imm5 - 32;
				}
				sprintf(assem, "ADD R%d, R%d, #%d", DR, SR1, imm5);
			}
			node->assembly = assem;
		} else { /* reaching here, opcode is not 0001 */
			node = search_opcode(memory, 0);
			if (node != NULL) { /* if the opcode is 0000 */
				int max = 7 + max_len_label;
				if (max < 12) max = 12;
				char* assem = malloc(max * sizeof(char));
				subopcode = node->contents / 512 % 8;
				if (subopcode == 4) { /* if subopcode is 100 */
					int imm9 = (node->contents) % 512;
					if (imm9 > 255) {
						imm9 = imm9 - 512;
					}
					struct row_of_memory* node_label = search_address(memory, node->address - imm9 + 1);
					if (node_label != NULL && node_label->label != NULL)
						printf(assem, "BRn %s", node_label->label);
					else sprintf(assem, "BRn #%d", imm9);
					node->assembly = assem;
				} else if (subopcode == 6) { /* if subopcode is 110 */
					/* TODO */
				}
			} else { /* if the opcode is not 0000 */
				/* TODO */
				memory = node->next; /* next time, search from the next node of current node */
			}
		}
		
		
	} while (1);
	return 0 ;
}
/*-------------end of disassembler ------------------------------*/






/* declarations of functions that must defined in lc4_loader.c */

int read_2bytes(FILE * infile) {
	int hex_read;
	int bytes_read = 0;
	hex_read = fgetc(infile);
	if (hex_read == EOF) return 0x10000;  /* reaching the end of the file */
	bytes_read = hex_read * 0x100;
	hex_read = fgetc(infile);
	if (hex_read == EOF) return 0x10000;  /* reaching the end of the file */
	bytes_read += hex_read;
	return bytes_read;
}

short unsigned int max_code_addr(struct row_of_memory* memory) {
	short unsigned int max = 0;
	while (memory != NULL) {
		if (memory->address > max)
			max = memory->address;
		memory = memory->next;
	}
	return max;
}

FILE* get_file() 
{	
	char* filename = NULL;
	/* request memory allocation */
	filename = malloc(100 * sizeof(char));
	if (filename == NULL) {
		fprintf(stderr, "Error: no space on the heap.\n");
		return NULL;
	}
	FILE * infile = NULL;
	printf("%s\n", "Please enter the name of the file you would like to convert:");
	scanf("%s", filename);
	infile = fopen(filename, "rb");   /* try to open binary file */
	free(filename);
	return infile ;
}

int parse_file (FILE* my_obj_file, struct row_of_memory** memory) 
{	
	int addr_field = 0;
	int n_field = 0;
	int content = 0;
	int read = 0;
	*memory = NULL;

	do {
		read = read_2bytes(my_obj_file);
		if (read == 0x10000) {
			return 2;
		}
		else if (read == 0xcade) {
			addr_field = read_2bytes(my_obj_file);
			n_field = read_2bytes(my_obj_file);
			while (n_field > 0) {
				content = read_2bytes(my_obj_file);
				struct row_of_memory* node = search_address(*memory, addr_field);
				if (node != NULL) {
					node->contents = content;
				} else {
					*memory = add_to_list(*memory, addr_field, content);
					addr_field++;
					n_field--;
				}
			}
		}
		else if (read == 0xdada) {
			addr_field = read_2bytes(my_obj_file);
			n_field = read_2bytes(my_obj_file);
			while (n_field > 0) {
				content = read_2bytes(my_obj_file);	
				struct row_of_memory* node = search_address(*memory, addr_field);
				if (node != NULL) {
					node->contents = content;
				} else {		
					*memory = add_to_list(*memory, addr_field, content);
					addr_field++;
					n_field--;
				}
			}
		}
		else if (read == 0xc3b7) {
			/*do {*/
				addr_field = read_2bytes(my_obj_file);
				if (addr_field == 0x10000) 
					return 2;
				n_field = read_2bytes(my_obj_file);
				struct row_of_memory* node = search_address(*memory, addr_field);
				if (node == NULL) {
					int max = max_code_addr(*memory);
					*memory = add_to_list(*memory, max + 1, 0);
					node = search_address(*memory, max + 1);
				}
				node->label = malloc((n_field + 1) * sizeof(char));
				if (node->label == NULL) {
					fprintf(stderr, "Error: no space on the heap.\n");
					return 2;
				}
				char letter = '\0';
				int i = 0;
				for (i = 0; i < n_field; i++) {
					letter = fgetc(my_obj_file);
					(node->label)[i] = letter;
				}
				(node->label)[n_field] = '\0';
			/*} while (1);*/
		}

	} while (1);

	return 0 ;
}

int main() {
	struct row_of_memory** memory = NULL;
	memory = malloc(sizeof(struct row_of_memory*));
	if (memory == NULL) {
		fprintf(stderr, "Error: no space on the heap.\n");
		return 1;
	}
	FILE* file = NULL;
	file = get_file();
	if (file == NULL) {
		printf("%s\n", "Failed to open the file.");
		free(memory);
		return 1;
	}
	int result = 0;
	result = parse_file(file, memory);
	if (fclose(file) != 0) { /* close files */
		printf("%s\n", "Failed to close the file.");
		free(memory);
		return 2;
	}
	result = reverse_assemble(*memory);
	print_list(*memory);
	*memory = delete_list(*memory);
	free(memory);
	return 0;
}
