#ifndef SL_MAP_H
#define SL_MAP_H

#include <stdint.h>
#include "tool.h"

#define TABLE_SIZE 512 // TODO: Make this dynamic

struct SL_key_value_pair {
    char		*key;
    uint64_t	value;
    struct SL_key_value_pair* next;
};

typedef struct SL_key_value_pair SL_key_value_pair;

struct SL_hash_map {
    struct SL_key_value_pair *table[TABLE_SIZE];
};

typedef struct SL_hash_map SL_hash_map;

uint64_t SL_hash(char* key);
SL_hash_map *SL_hash_map_new(void);
void SL_hash_map_insert(SL_hash_map *map, char *key, uint64_t value);
uint64_t *SL_hash_map_get(SL_hash_map *map, char *key);

#endif // SL_MAP_H
