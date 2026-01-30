#include "map.h"
#include <stdlib.h>
#include <string.h>

typedef struct MapEntry {
    void* key;
    void* value;
    struct MapEntry* next;
} MapEntry;

struct Map {
    MapEntry** buckets;
    size_t capacity;
    size_t size;
    MapHash hash;
    MapCompare compare;
    MapFree freeKey;
    MapFree freeValue;
};

// Default hash function for strings (djb2)
static size_t stringHash(const void* key) {
    const char* str = (const char*)key;
    size_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

// Default compare function for strings
static int stringCompare(const void* key1, const void* key2) {
    return strcmp((const char*)key1, (const char*)key2);
}

// Default free function (does nothing, for non-malloced pointers)
static void noFree(void* ptr) {
    (void)ptr; // unused
}

Map* mapCreate(size_t initialCapacity, MapHash hash, MapCompare compare, MapFree freeKey, MapFree freeValue) {
    if (initialCapacity == 0) return NULL;
    Map* map = malloc(sizeof(Map));
    if (!map) return NULL;
    map->buckets = calloc(initialCapacity, sizeof(MapEntry*));
    if (!map->buckets) {
        free(map);
        return NULL;
    }
    map->capacity = initialCapacity;
    map->size = 0;
    map->hash = hash;
    map->compare = compare;
    map->freeKey = freeKey ? freeKey : noFree;
    map->freeValue = freeValue ? freeValue : noFree;
    return map;
}

Map* mapCreateStringMap(size_t initialCapacity) {
    return mapCreate(initialCapacity, stringHash, stringCompare, free, free);
}

void mapDestroy(Map* map) {
    if (!map) return;
    mapClear(map);
    free(map->buckets);
    free(map);
}

bool mapPut(Map* map, void* key, void* value) {
    if (!map || !key) return false;
    size_t index = map->hash(key) % map->capacity;
    MapEntry* entry = map->buckets[index];
    while (entry) {
        if (map->compare(entry->key, key) == 0) {
            // Update value, free old if needed
            if (map->freeValue) map->freeValue(entry->value);
            entry->value = value;
            return true;
        }
        entry = entry->next;
    }
    // New entry
    MapEntry* newEntry = malloc(sizeof(MapEntry));
    if (!newEntry) return false;
    newEntry->key = key;
    newEntry->value = value;
    newEntry->next = map->buckets[index];
    map->buckets[index] = newEntry;
    map->size++;
    return true;
}

void* mapGet(Map* map, void* key) {
    if (!map || !key) return NULL;
    size_t index = map->hash(key) % map->capacity;
    MapEntry* entry = map->buckets[index];
    while (entry) {
        if (map->compare(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

bool mapRemove(Map* map, void* key) {
    if (!map || !key) return false;
    size_t index = map->hash(key) % map->capacity;
    MapEntry* entry = map->buckets[index];
    MapEntry* prev = NULL;
    while (entry) {
        if (map->compare(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                map->buckets[index] = entry->next;
            }
            map->freeKey(entry->key);
            map->freeValue(entry->value);
            free(entry);
            map->size--;
            return true;
        }
        prev = entry;
        entry = entry->next;
    }
    return false;
}

bool mapContains(Map* map, void* key) {
    return mapGet(map, key) != NULL;
}

size_t mapSize(Map* map) {
    return map ? map->size : 0;
}

void mapClear(Map* map) {
    if (!map) return;
    for (size_t i = 0; i < map->capacity; i++) {
        MapEntry* entry = map->buckets[i];
        while (entry) {
            MapEntry* next = entry->next;
            map->freeKey(entry->key);
            map->freeValue(entry->value);
            free(entry);
            entry = next;
        }
        map->buckets[i] = NULL;
    }
    map->size = 0;
}

bool mapResize(Map* map, size_t newCapacity) {
    if (!map || newCapacity == 0 || newCapacity < map->size) return false;
    MapEntry** newBuckets = calloc(newCapacity, sizeof(MapEntry*));
    if (!newBuckets) return false;
    // Rehash all entries
    for (size_t i = 0; i < map->capacity; i++) {
        MapEntry* entry = map->buckets[i];
        while (entry) {
            MapEntry* next = entry->next;
            size_t newIndex = map->hash(entry->key) % newCapacity;
            entry->next = newBuckets[newIndex];
            newBuckets[newIndex] = entry;
            entry = next;
        }
    }
    free(map->buckets);
    map->buckets = newBuckets;
    map->capacity = newCapacity;
    return true;
}
