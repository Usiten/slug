#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "map.h"

uint64_t SL_hash(char *key) 
{
    uint64_t hash = 0;
    while (*key) {
        hash = (hash * 31) + (*key);
        key++;
    }
    return hash % TABLE_SIZE;
}

SL_hash_map *SL_hash_map_new(void)
{
	SL_hash_map *map = calloc(1, sizeof(SL_hash_map));
	SL_ALLOC_CHECK(map)
	return map;
}

void SL_hash_map_insert(SL_hash_map *map, char *key, uint64_t value)
{
	uint64_t index = SL_hash(key);

	SL_key_value_pair *kv = calloc(1, sizeof(SL_key_value_pair));
	SL_ALLOC_CHECK(kv)

	kv->key = SL_strdup(key);
	kv->value = value;
	kv->next = NULL;

	if (map->table[index] == NULL) {
		map->table[index] = kv;
	}
	else {
		SL_key_value_pair *current = map->table[index];
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = kv;
	}
}

uint64_t *SL_hash_map_get(SL_hash_map *map, char *key) 
{
	uint64_t index = SL_hash(key);

	SL_key_value_pair *current = map->table[index];
	while (current != NULL) {
		if (strcmp(current->key, key) == 0) {
			return &(current->value);
		}
		current = current->next;
	}

	return NULL;
}