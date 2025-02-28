#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "assembler/hashmap.h"

#define INITIAL_CAPACITY 100

MapEntry* create_map_entry(char* key, void* value){
	MapEntry* entry = (MapEntry*) malloc(sizeof(MapEntry));

	if (entry == NULL) {
		// Print error message and exit if memory allocation fails
		fprintf(stderr, "Error: failed to allocate memory for MapEntry\n");
		exit(1);
	}
	
	entry->key = key;
	entry->value = value;
	entry->next = NULL;

	return entry;
}

MapEntry* create_map_entry_with_next(char* key, void* value, MapEntry* next){
	MapEntry* entry = (MapEntry*) malloc(sizeof(MapEntry));

	if (entry == NULL) {
		// Print error message and exit if memory allocation fails
		fprintf(stderr, "Error: failed to allocate memory for MapEntry\n");
		exit(1);
	}
	
	entry->key = key;
	entry->value = value;
	entry->next = next;

	return entry;
}

uint64_t hash_function(HashMap* hm, const char* key){
	uint64_t hash = 5381;
	uint64_t c;

	// Compute the hash value for the label
	while ((c = *key++)){
		if (isupper(c)){
			c = c + 32;
		}

		hash = ((hash << 5) + hash) + c;
	}

	return hash % hm->capacity;
}

HashMap* create_hashmap(){
	HashMap* hm = malloc(sizeof(HashMap));

	if (hm == NULL) {
		// Print error message and exit if memory allocation fails
		fprintf(stderr, "Failed to allocate memory for HashMap\n");
		exit(1);
	}
	
	hm->capacity = INITIAL_CAPACITY;
	hm->data = (MapEntry**) calloc(INITIAL_CAPACITY, sizeof(MapEntry*));

	return hm;
}

void hashmap_insert(HashMap* hm, char* key, void* value){
	if(hm == NULL){
		return;
	}

	uint64_t hash = hash_function(hm, key);
	MapEntry* currentEntry = hm->data[hash];

	if(currentEntry != NULL){
		// Traverse the linked list to check for duplicate labels
		while (currentEntry != NULL) {
			if (strcmp(currentEntry->key, key) == 0) {
				return;
			}

			currentEntry = currentEntry->next;
		}

		// Insert the new label entry at the beginning of the list
		MapEntry* entry = create_map_entry_with_next(key, value, hm->data[hash]);
		hm->data[hash] = entry;
	}
	else{
		// Insert the new label entry if no collision
		MapEntry* entry = create_map_entry(key, value);
		hm->data[hash] = entry;
	}
}

void* hashmap_get(HashMap* hm, char* key){
	if(hm == NULL){
		return NULL;
	}

	uint64_t hash = hash_function(hm, key);
	MapEntry* currentEntry = hm->data[hash];

	// Traverse the linked list to find the label
	while(currentEntry != NULL){
		if(strcmp(currentEntry->key, key) == 0){
			return currentEntry->value;
		}
		
		currentEntry = currentEntry->next;
	}

	return NULL;
}

void print_hashmap(HashMap* hm, void (*print)(void*)){
	if(hm == NULL){
		return;
	}

	// Iterate through the hash map and print all entries
	for(int i = 0; i < hm->capacity; i++){
		if(hm->data[i] != NULL){
			MapEntry* currentEntry = hm->data[i];

			while(currentEntry != NULL){
				printf("%s: ", currentEntry->key);
				print(currentEntry->value);

				currentEntry = currentEntry->next;
			}
		}
	}
}

void destroy_hashmap(HashMap* hm, void (*destroy)(void*)){
	if(hm == NULL){
		return;
	}

	// Iterate through the hash map and free all entries
	for(int i = 0; i < hm->capacity; i++){
		if(hm->data[i] != NULL){
			MapEntry* currentEntry = hm->data[i];

			while(currentEntry != NULL){
				MapEntry* temp = currentEntry;
				currentEntry = currentEntry->next;
				destroy(temp->value);
				free(temp);
			}
		}
	}

	free(hm->data);
	free(hm);
}