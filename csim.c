// Cache Simulator
// Author: Griffin Bishop
// WPI Username (login id): grbishop

struct operation{
    long address;
    char op_type;
};
typedef struct operation Operation;

typedef struct{
    unsigned long long tag;
    int set_number;
    //int offset;
} Address_info;

typedef struct{
    int valid;
    int tag;
    int used;
    char* blockPtr; // Pointer to the array of memory
} Line;

typedef struct{
    // A set is a list of lines
    int last_recently_used;
    Line** lines;
} Set;

// A cache is an array of sets of lines. But we don't know how many sets or lines there
// are at compile time so we say a cache is a list of sets, and a set is a list
// of lines.
typedef Set** Cache;


#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// globals
int s;
int E;
int b;
int time = 0;
int cache_is_full = 0;
int misses = 0;
int hits = 0;
int evictions = 0;

/* Function create_cache
 * Given s, E, and b, initalize and malloc 
 * memory for an empty cache.
 * Return a pointer to the cache.
 */
Cache create_cache(int num_set_bits, int lines_per_set, int num_block_bits){
    // Initalize cache
    int num_sets = pow(2, num_set_bits);
    Cache cache = malloc(sizeof(Set*)*num_sets);    

    int i;
    for(i=0;i<num_sets;i++){
        Set* aset = malloc(sizeof(Set));
        aset->last_recently_used = 1;
        aset->lines = malloc(sizeof(Line*)*lines_per_set);
        
        int j;
        for(j=0;j<lines_per_set;j++){
            Line* linePtr = malloc(sizeof(Line));
	    aset->lines[j] = linePtr;

            linePtr->valid = 0;
            linePtr->tag   = -1;
	    linePtr->used  = 0;
            // Don't allocate anything for a block; not actually used in simulator.
        }
	cache[i] = aset;
    }
    return cache;
}

/* Function get_address_info
 * Given an unsigned long int pointer to an address,
 * and the number of set bits, and the number of block bits,
 * produces a pointer to a address info struct.
 */
Address_info* get_address_info(unsigned long long *address_ptr, int s, int b){
    Address_info* address_info_ptr = malloc(sizeof(Address_info));
    
    unsigned long long address = *address_ptr;
    // Addresses are in the form: t tag bits, s set bits, b offset bits
    // | t |  s  | b |
    // We need to isolate these parts
    // Tag is easy, just shift over s+b
    unsigned long long tag = address >> (s + b);

    // b doesn't concern the simulation

    // For s, just shift | s | b | over to clear everything but it, 
    // then shift back, but b bits more, so that we're only left with | s |
    int shift_by = (64 - (s + b));
    unsigned long long shifted = address << shift_by;
    unsigned long long set_number = shifted >> (shift_by + b);

    address_info_ptr->tag = tag;
    address_info_ptr->set_number = set_number;

    return address_info_ptr;
}

/* Function find_line()
 * Given a set in which we will search for the line with the tag given.
 * Produces the line with the tag given or -1 if there is no such line.
 */
int find_line(Set* in_set, int tag){
    int line_index = 0;

    // Loop through the lines
    for(line_index=0;line_index<E;line_index++){
	Line* aline = in_set->lines[line_index];

	if(aline->valid && aline->tag == tag){
	    // We have a hit!
	    return line_index;
	}
    }
    return -1; // If we get here, there is no line with that tag.
}

/* Function load_line()
 * Given a set pointer and a tag,
 * Replaces the line at (last_recently_used) and activates it.
 */
void load_line(Set* in_set, int tag, int line_index){
    time++; // Increment time every time we load.
    Line* line = in_set->lines[line_index];
    line->used  = time;
    line->valid = 1;
    line->tag   = tag;
}

/* Function find_evict_line()
 * Produces the index of the line to evict based
 * on the one that has been used least recently.
 */
int find_evict_line(Set* set_ptr){
    int lru_time = set_ptr->lines[0]->used;
    int lru_index = 0;     

    int i;
    for(i=0;i<E;i++){
	Line* aline = set_ptr->lines[i];
	printf("\nI: %d -- TimeUsed: %d -- Tag: %d\n", i, aline->used, aline->tag);
	if(aline->used < lru_time){
	    lru_time = aline->used;
	    lru_index = i;
	}
    }
    return lru_index;
}

/* Function find_empty_line()
 * Given a set pointer, returns the index 
 * of an empty line in the set. If no such line exists, 
 * returns -1.
 */
int find_empty_line(Set* aset){
    int i;
    for(i=0;i<E;i++){
	Line* aline = aset->lines[i];
	if(!aline->valid){
	    return i;
	}
    }
    return -1; // No invalid was found; return -1 (error)
}

/* Function is_set_full
 * Given a set pointer, returns true if there are
 * no invalid lines. Returns false if there is atleast
 * 1 invalid line.
 */
int is_set_full(Set* aset){
    int i;
    for(i=0;i<E;i++){
	Line* aline = aset->lines[i];
	if(!aline->valid){
	    return 0;
	}
    }
    return 1; // No invalid was found; return true
}

/* Function simulate()
 * Given a Cache struct and an Address_info struct (tag/set to be operated on)
 * Produces an integer code representing the outcome of the operation.
 * 0 : Miss
 * 1 : Hit
 * 2 : Miss Eviction
 */
int simulate(Cache cache, Address_info* address){

    // Get the set at the index given in the address info
    Set* set_ptr = cache[address->set_number];
    int line_index = find_line(set_ptr, address->tag);
    if(line_index != -1){
     	// There is a line with the tag; Hit!
	Line* hit_line = set_ptr->lines[line_index];
	hit_line->used = time;
	time++;
	hits++;
	return 1;
    } else {
	// No line with that tag. Miss.
	misses++;
	if(is_set_full(set_ptr)){
	    // We need to evict!
	    int evict_index = find_evict_line(set_ptr);
            load_line(set_ptr, address->tag, evict_index);
	    evictions++;
	    return 2;
	} else {
	    // Cache is not full, so it's just a normal miss
	    int empty_index = find_empty_line(set_ptr);
	    load_line(set_ptr, address->tag, empty_index);
	    return 0;
	}
    }

    return -1;// something went wrong.
}

/* Function simulate_helper
 * Calls simulate() based on the (char action) given.
 * If L - (Load), call normally.
 * If S - (Store), call normally.
 * If M - (Modify), call twice, recording result.
 * Returns an integer reflecting the result of simulate()
 */
int simulate_helper(Cache cache, Address_info* address, char action){
    int sim_result;
    if(action == 'L' || action == 'S'){
 	sim_result = simulate(cache, address);
    } else if(action == 'M'){
	sim_result = simulate(cache, address);
	sim_result += simulate(cache, address);
    }
    return sim_result;
}


/* Function print_result
 * Prints the label for the given result code
 * Does not print an endline or newline.
 */
void print_result(int result){
    if(result == 0){
        printf(" miss");
    } else if(result == 1){
        printf(" hit");
    } else if(result == 2){
        printf(" miss eviction");
    }
}

int main(int argc, char* argv[]) {
    int opt; 
    int required_flags = 0;
    char* filename; 
    int num_set_bits;
    int lines_per_set;
    int num_block_bits;
    int verbose_flag = 0; // Default: Disabled = 0. 

    while ((opt = getopt(argc, argv, "hs:E:b:vt:")) != -1) {
        switch(opt) {
        case 's':
	    num_set_bits = atoi(optarg);
	    s = num_set_bits;
	    required_flags += 1;
	    break;
	case 'E':
	    lines_per_set = atoi(optarg);
	    E = lines_per_set;
	    required_flags += 1;
 	    break;
        case 'b':
            num_block_bits = atoi(optarg);
	    b = num_block_bits;
	    required_flags += 1;
	    break;
    	case 'v':
            verbose_flag = 1;
	    break;
  	case 't':
	    filename = optarg;
	    required_flags += 1;
	    break;
	default:
	    printf("Usage: ./csim [-h] [-v] [-s bits] [-E lines]");
 	    printf(" [-b bits] [-t filename]\n");
 	    exit(EXIT_FAILURE);
        }
    }
	
    if(required_flags < 4) {
        printf("Usage: ./csim [-h] [-v] [-s bits] [-E lines]");
 	printf(" [-b bits] [-t filename]\n");
 	exit(EXIT_FAILURE);
    }
    
    /////////////// We have arguments, now parse the tracefile ////////////////

    FILE* trace = fopen(filename,"r");
    if(trace){
        //printf("File opened successfully.\n");
    } else {
        fprintf(stderr, "Could not open file: %s.\n", filename);
        exit(EXIT_FAILURE);
    } 


    // Build a cache structure on which the simulation will be run.
    Cache cache = create_cache(num_set_bits, lines_per_set, num_block_bits);


    char* type_char_ptr = malloc(sizeof(char));
    // Is unsigned long because int is too small to hold an 8 character hex val
    unsigned long long* address_ptr = malloc(sizeof(unsigned long long));
    int* size_ptr = malloc(sizeof(int));

    int matched;
 
    while((matched = fscanf(trace, " %c %llx,%d", 
	type_char_ptr, address_ptr, size_ptr)) == 3) 
    {
	if(*type_char_ptr == 'I') { // We don't care about I's
            goto ALLOCATE; // Skip over the structure allocation
        }

	Address_info* address = get_address_info(address_ptr, 
			num_set_bits, num_block_bits);
	
	Operation* op  =  malloc(sizeof(Operation));
	op->address    = *address_ptr;
	op->op_type    = *type_char_ptr;	

	if(verbose_flag){
	    printf("Type: %c, Address: %lx, Set: %d, Tag: %lld", op->op_type,
		op->address, address->set_number, address->tag);
	}

	// If the action is 'M', simulate twice, otherwise once.
        int times = (op->op_type == 'M') ? 2 : 1;
	while (times > 0){
	    int result = simulate(cache, address);
	    if(verbose_flag){
	       	print_result(result);
	    }
	    times--;
	}

	if(verbose_flag) printf("\n");
	
	ALLOCATE:
	// Free and Allocate space for the next thing.
	free(type_char_ptr);
	free(address_ptr);
        type_char_ptr = malloc(sizeof(char));
        address_ptr = malloc(sizeof(unsigned long));
    }

    fclose(trace);
    printSummary(hits, misses, evictions);
    return 0;
}


