#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>


#include "ai.h"
#include "utils.h"
#include "hashtable.h"
#include "priority_queue.h"

/**
 * Retrieve solution and return a string containing the squence of moves
*/
char* save_solution( node_t* solution_node ){
	node_t* n = solution_node;
	char *solution_string = malloc(sizeof(char) * solution_node->depth+1);
	solution_string[n->depth] = '\0';
	while (n->parent != NULL)
	{

		switch (n->move) {
		case up:
			if (n->parent->state.map[n->state.player_y][n->state.player_x] == '$')
				solution_string[n->depth-1] = 'U';
			else
				solution_string[n->depth-1] = 'u';
			break;

		case down:  
			if (n->parent->state.map[n->state.player_y][n->state.player_x] == '$')
				solution_string[n->depth-1] = 'D';
			else     
				solution_string[n->depth-1] = 'd';
			break;

		case left:  
			if (n->parent->state.map[n->state.player_y][n->state.player_x] == '$')
				solution_string[n->depth-1] = 'L';
			else    
				solution_string[n->depth-1] = 'l';
			break;

		case right: 
			if (n->parent->state.map[n->state.player_y][n->state.player_x] == '$')
				solution_string[n->depth-1] = 'R';
			else       
				solution_string[n->depth-1] = 'r';
			break;

		}

		n = n->parent;
	}
	return solution_string;
}

/**
 * Copy a src into a dst state
*/
void copy_state(sokoban_t* init_data, state_t* dst, state_t* src){

	dst->map = malloc(sizeof(char *) * init_data->lines);
	for ( int i = 0; i < init_data->lines; ++i ){
		int width = strlen(src->map[i]) + 1;
		dst->map[i] = malloc(width);    
		memcpy(dst->map[i], src->map[i], width);
	}

	dst->player_x = src->player_x;

	dst->player_y = src->player_y;

}

/**
 * Create the initial node
*/
node_t* create_init_node( sokoban_t* init_data ){
	node_t * new_n = (node_t *) malloc(sizeof(node_t));
	new_n->parent = NULL;	
	new_n->priority = 0;
	new_n->depth = 0;
	new_n->num_childs = 0;
	new_n->move = -1;
	new_n->state.map = malloc(sizeof(char *) * init_data->lines);
	for (int i = 0; i < init_data->lines; ++i)
	{
		int width = strlen(init_data->map[i]) + 1;
		new_n->state.map[i] = malloc(width);    
		memcpy(new_n->state.map[i], init_data->map[i], width);
	}

	new_n->state.player_x = init_data->player_x;

	new_n->state.player_y = init_data->player_y;
	return new_n;
}

/**
 * Create the a node from a parent node
*/
node_t* create_node( sokoban_t* init_data, node_t* parent ){
	node_t * new_n = (node_t *) malloc(sizeof(node_t));
	new_n->parent = parent;
	new_n->depth = parent->depth + 1;
	copy_state(init_data, &(new_n->state), &(parent->state));
	return new_n;
	
}

/**
 * Apply an action to node n, create a new node resulting from 
 * executing the action, and return if the player moved
*/
bool applyAction(sokoban_t* init_data, node_t* n, node_t** new_node, move_t action ){

	bool player_moved = false;

    *new_node = create_node( init_data, n );
    (*new_node)->move = action;
	(*new_node)->priority =  -(*new_node)->depth;

    player_moved = execute_move_t( init_data, &((*new_node)->state), action );	

	return player_moved;

}

/**
 * Book keeping variable and function to free memory once the solver finishes
*/
node_t** expanded_nodes_table;
unsigned expanded_nodes_table_size = 10000000; //10M
void update_explore_table(node_t* n, unsigned expanded_nodes ){
    if( expanded_nodes > expanded_nodes_table_size - 1){
	expanded_nodes_table_size *= 2;
	expanded_nodes_table = realloc( expanded_nodes_table, sizeof(node_t*) * expanded_nodes_table_size );
	
    }
    
    expanded_nodes_table[ expanded_nodes ] = n;

}

void free_memory(unsigned expanded_nodes ){
	for( unsigned i = 0; i < expanded_nodes; i++){	
		free(expanded_nodes_table[ i ]);
    }
	free(expanded_nodes_table);
}

/**
 * Given a 2D map, returns a 1D map
*/
void flatten_map( sokoban_t* init_data, char **dst_map, char **src_map){

	int current_i = 0;
	for (int i = 0; i < init_data->lines; ++i)
	{
		int width = strlen(src_map[i]);
		for ( int j = 0; j < width; j++ ){
			(*dst_map)[current_i]  = src_map[i][j];
			current_i++;
		}
	}
}

/**
 * Find a solution by exploring all possible paths
 */
void find_solution(sokoban_t* init_data, bool show_solution)
{
	// Keep track of solving time
	clock_t start = clock();
	
	// Solution String containing the sequence of moves
	char* solution = NULL;

	HashTable hashTable;
	struct heap pq;

	// Statistics
	unsigned generated_nodes = 0;
	unsigned expanded_nodes = 0;
	unsigned duplicated_nodes = 0;
	int max_depth = 0;
	unsigned solution_size = 0;

	// Choose initial capacity of PRIME NUMBER 
	// Specify the size of the keys and values you want to store once 
	// The Hash Table only accept a 1D key and value.
	ht_setup( &hashTable, sizeof(int8_t) * init_data->num_chars_map, sizeof(int8_t) * init_data->num_chars_map, 16769023);

	

	// Data structure to create a 1D representation of the map
	// Needed to interact with the hash table
	char* flat_map = calloc( init_data->num_chars_map, sizeof(char));

	// Initialize heap
	heap_init(&pq);

	// Initialize expanded nodes table. 
	// This table will be used to free your memory once a solution is found
	expanded_nodes_table = (node_t**) malloc( sizeof(node_t*) * expanded_nodes_table_size ); 


	// Add the initial node
	node_t* n = create_init_node( init_data );
	
	// Use the max heap API provided in priority_queue.h
	heap_push(&pq,n);
	
	/** 
	 * FILL IN THE GRAPH ALGORITHM 
	 * */
	// Lines 6-9: Takes the highest priority item out of the queue and inserts its address to expanded_nodes_table_size
	 while(pq.count > 0){
        n = heap_delete(&pq);
        expanded_nodes++;
        expanded_nodes_table[expanded_nodes-1] = n;
		// Lines 10-14: Checks if winning condition is satisfied  
        if(winning_condition(init_data, &(n->state))){
            solution = save_solution(n);
            solution_size = n->depth;
            break;
        }
		// Lines 15-17: Creates a new node for every possible moves
        for(int i = left; i <= down; i++ ){
            node_t* child_node;
            bool playerMoved = applyAction(init_data, n, &child_node, i);
            generated_nodes++;
			// Lines 18-21: Checks if the node created is valid, frees the node and it's associated memories if it is invalid
            if(!playerMoved || simple_corner_deadlock(init_data, &(child_node->state))){
				for (int j = 0; j < init_data->lines; ++j){
					free(child_node->state.map[j]);
				}
				free(child_node->state.map);
				free(child_node);
                continue;
            }
			// Line 22: Converts the map into a 1D array
            flatten_map(init_data, &flat_map, child_node->state.map);

			// Lines 23-27: Checks whether the map is a duplicate, frees the node if it is a duplicate
            if(ht_contains(&hashTable, flat_map)){
                duplicated_nodes++;
				for (int j = 0; j < init_data->lines; ++j){
					free(child_node->state.map[j]);
				}
				free(child_node->state.map);
                free(child_node);
                continue;
            }
			// Line 28: Inserts the map into the hashtable so that it can be checked for duplicates in future operations
            ht_insert(&hashTable, flat_map, flat_map);
			// Line 29: Inserts the new node into the priority queue
            heap_push(&pq,child_node);
        }
    }
	
	

	//----------------------------
	
	// Frees the nodes and its associated memories for each node still in the priority queue
	while(pq.count > 0){
        n = heap_delete(&pq);
		for (int j = 0; j < init_data->lines; ++j){
			free(n->state.map[j]);
		}
		free(n->state.map);
        free(n);
    }
	// Frees memory used to store the map data
	for (int i = 0; i <init_data->lines ; i++) {
        free(init_data->map[i]);
        free(init_data->map_save[i]);
    }
	free(init_data->map);
    free(init_data->map_save);
	free(init_data->buffer);

	// Frees the maps inside each node
	for( unsigned i = 0; i < expanded_nodes; i++){
		for (int j = 0; j < init_data->lines; ++j){
			free(expanded_nodes_table[i]->state.map[j]);
		}
		free(expanded_nodes_table[i]->state.map);
	}
	free(pq.heaparr);
    
	// Free Memory of HashTable, Explored and flatmap
	// ------------------------
	ht_clear(&hashTable);
	ht_destroy(&hashTable);
	free_memory(expanded_nodes);
	free(flat_map);
	//----------------------------

	// Stop clock
	clock_t end = clock();
	double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

	// Show Soltion	
	if( show_solution && solution != NULL ) play_solution( *init_data, solution);

	endwin();

	if( solution != NULL ){
		printf("\nSOLUTION:                               \n");
		printf( "%s\n\n", solution);
		FILE *fptr = fopen("solution.txt", "w");
		if (fptr == NULL)
		{
			printf("Could not open file");
			return ;
		}
		fprintf(fptr,"%s\n", solution);
		fclose(fptr);
		
		free(solution);
	}


	printf("STATS: \n");
	printf("\tExpanded nodes: %'d\n\tGenerated nodes: %'d\n\tDuplicated nodes: %'d\n", expanded_nodes, generated_nodes, duplicated_nodes);
	printf("\tSolution Length: %d\n", solution_size);
	printf("\tExpanded/seconds: %d\n", (int)(expanded_nodes/cpu_time_used) );
	printf("\tTime (seconds): %f\n", cpu_time_used );
	
	
}

void solve(char const *path, bool show_solution)
{
	/**
	 * Load Map
	*/
	sokoban_t sokoban = make_map(path, sokoban);
	
	/**
	 * Count number of boxes and Storage locations
	*/
	map_check(sokoban);

	/**
	 * Locate player x,y position
	*/
	sokoban = find_player(sokoban);
	
	sokoban.base_path = path;

	find_solution(&sokoban, show_solution);

}
