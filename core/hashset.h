#ifndef HASHSET_DECLARATION
#define HASHSET_DECLARATION

#include <stdbool.h>
#include "base.h"
#include "str.h"

typedef struct {
  String key;
} HashEntry;

typedef struct {
  u64 capacity;
  u64 size;
  void* entries;
  bool* is_taken;
} HashSet;

static u64 hash(String);

HashSet *HashSetNew(u64 capacity);
bool HashSetContains(HashSet *, String);
void HashSetInsert(HashSet *, String);
HashEntry *HashSetDelete(HashSet *, String);
void HashSetFree(HashSet *);


static u64 hash(String key) {
    u64 sum_product = 0;
    for (size_t i = 0; i < key.length; i++) {
        sum_product += key.data[i];
        sum_product *= key.data[i];
    }
    return sum_product;
}

HashSet *HashSetNew(u64 capacity) {
    HashSet *hashset = malloc(sizeof(HashSet));
    if (hashset == NULL) {
        return NULL;
    }
    hashset->capacity = capacity;
    hashset->size = 0;
    hashset->entries = calloc(hashset->capacity, sizeof(HashEntry));
    hashset->is_taken = calloc(hashset->capacity, sizeof(int));
    return hashset;
}

bool
HashSetContains(
    HashSet *hashset, 
    String key
) {
    if (hashset == NULL || StrIsNull(&key)) {
        return NULL;
    }

    u64 hash_of_key = hash(key);
    u64 index = hash_of_key % hashset->capacity;

    HashEntry *he = hashset->entries + index;

    do {
        if (StrEqual(he->key, key)) {
            return true;
        }
        index++;
        he = hashset->entries + index;
    } while(index < hashset->size);
    return false;
}

void
HashSetInsert(
    HashSet *hashset,
    String key
) {
    if (hashset == NULL // The hashset is null
        || StrIsNull(&key) // The key is null
        || HashSetContains(hashset, key) // The already in the table
        ) {
            return;
        }

    f64 load_factor = (f64)(hashset->size + 1) / (f64)hashset->capacity;

    if (load_factor > 0.6) {
        HashEntry *new_entries = calloc(hashset->capacity * 2, sizeof(HashEntry));
        if (new_entries == NULL) {
            perror("calloc failed while allocating space for another entries array\n");
            exit(1);
        }
        bool *new_taken = calloc(hashset->capacity * 2, sizeof(bool));
        if (new_taken == NULL) {
            perror("calloc failed while allocating space for another is_taken array\n");
            exit(1);
        }
        for (int i = 0; i < hashset->capacity; i++) {
            if (hashset->is_taken[i] == 0) {
                continue;
            }
            HashEntry *original = hashset->entries + i;
            u64 hash_of_entry = hash(original->key);
            u64 index_of_entry = hash_of_entry % (hashset->capacity * 2);
            while (
                index_of_entry < hashset->capacity * 2
                && new_taken[index_of_entry] == 1) {
                index_of_entry++;
            }

            HashEntry *he = new_entries + index_of_entry;
            if (he != NULL) {
            }
            he->key = original->key;
            new_taken[index_of_entry] = 1;
            
        }
        hashset->capacity *= 2;
        free(hashset->entries);
        hashset->entries = new_entries;
        free(hashset->is_taken);
        hashset->is_taken = new_taken;
    }
    u64 hash_of_target = hash(key);
    u64 index = hash_of_target % hashset->capacity;

    while (
        index < hashset->capacity
        && hashset->is_taken[index]) {
        index++;
    }

    if (index < hashset->capacity) {
        HashEntry *he = hashset->entries + index;
        
        he->key = key;
        hashset->is_taken[index] = true;
        hashset->size++;
    }
}

HashEntry *
HashSetDelete(
    HashSet *hashset,
    String key
) {
    if (
        hashset == NULL
        || StrIsNull(&key)
        || !HashSetContains(hashset, key)) {
            return NULL;
        }
    
    u64 hash_of_key = hash(key);
    u64 index = hash_of_key % hashset->capacity;

    HashEntry *he = hashset->entries + index;

    while (
        index < hashset->capacity 
        && hashset->is_taken[index] == 1
        && StrEqual(he->key, key) != 0) {
            index++;
            he = hashset->entries + index;
        } 
    
    if (hashset->is_taken[index] == 1) {
        hashset->is_taken[index] = 0;
        return he;
    }

    return NULL;

}

void
HashSetFree(
    HashSet *hashset
) {
    for (u32 i = 0; i < hashset->capacity; i++) {
        if (hashset->is_taken[i] == 0) continue;
        StrFree(((HashEntry *)(hashset->entries + i))->key);
    }
    free(hashset->entries);
    free(hashset->is_taken);
    free(hashset);
}

#endif