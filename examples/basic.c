#include <map.h>
#include <stdio.h>
#include <string.h>

// Simple hash for strings
size_t stringHash(const void* key) {
    const char* str = (const char*)key;
    size_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// Compare for strings
int stringCompare(const void* key1, const void* key2) {
    return strcmp((const char*)key1, (const char*)key2);
}

int main() {
    // Create a map with string keys and string values, initial capacity 10
    // Using NULL for free functions since we use string literals
    Map* map = mapCreate(10, stringHash, stringCompare, NULL, NULL);
    if (map == NULL) {
        printf("Failed to create map\n");
        return -1;
    }

    // Put some key-value pairs
    if (!mapPut(map, "key1", "value1")) {
        printf("Failed to put key1\n");
        mapDestroy(map);
        return -1;
    }
    if (!mapPut(map, "key2", "value2")) {
        printf("Failed to put key2\n");
        mapDestroy(map);
        return -1;
    }

    // Get a value
    char* value = (char*)mapGet(map, "key1");
    if (value != NULL) {
        printf("Got value for key1: %s\n", value);
    } else {
        printf("Key1 not found\n");
    }

    // Check if contains key
    if (mapContains(map, "key2")) {
        printf("Map contains key2\n");
    }

    // Remove a key
    if (mapRemove(map, "key1")) {
        printf("Removed key1\n");
    }

    // Get size
    printf("Map size: %zu\n", mapSize(map));

    // Clear the map
    mapClear(map);
    printf("Map cleared, size: %zu\n", mapSize(map));

    // Destroy the map
    mapDestroy(map);
    return 0;
}