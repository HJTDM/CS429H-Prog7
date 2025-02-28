#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h>

/**
 * @brief Structure representing a map entry.
 */
typedef struct MapEntry{
	char* key; /**< key of the entry */
	void* value; /**< value of the entry */
	struct MapEntry* next; /**< pointer to the next entry */
} MapEntry;

/**
 * @brief Creates a new map entry.
 * 
 * @param key the key for the entry
 * @param value the value for the entry
 * @return Pointer to the newly created map entry.
 */
MapEntry* create_map_entry(char* key, void* value);

/**
 * @brief Creates a new map entry with a next pointer.
 * 
 * @param key the key for the entry
 * @param value the value for the entry
 * @param next pointer to the next map entry
 * @return Pointer to the newly created map entry.
 */
MapEntry* create_map_entry_with_next(char* key, void* value, MapEntry* next);

/**
 * @brief Structure representing a hash map.
 */
typedef struct HashMap{
	uint32_t capacity; /**< capacity of the hash map */
	MapEntry** data; /**< array of map entries */
} HashMap;

/**
 * @brief Creates a new hash map.
 * 
 * @return Pointer to the newly created hash map.
 */
HashMap* create_hashmap();

/**
 * @brief Inserts a key-value pair into the hash map.
 * 
 * @param hm pointer to the hash map
 * @param key the key to insert
 * @param value the value to insert
 */
void hashmap_insert(HashMap* hm, char* key, void* value);

/**
 * @brief Retrieves a value from the hash map by key.
 * 
 * @param hm pointer to the hash map
 * @param key the key to search for
 * @return Pointer to the value associated with the key, or NULL if not found.
 */
void* hashmap_get(HashMap* hm, char* key);

/**
 * @brief Prints the hash map.
 * 
 * @param hm pointer to the hash map
 * @param print function pointer to print the value
 */
void print_hashmap(HashMap* hm, void (*print)(void*));

/**
 * @brief Destroys the hash map and frees memory.
 * 
 * @param hm pointer to the hash map
 * @param destroy function pointer to destroy the value
 */
void destroy_hashmap(HashMap* hm, void (*destroy)(void*));

#endif