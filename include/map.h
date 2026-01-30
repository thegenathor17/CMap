#ifndef COMMON_MAP_H
#define COMMON_MAP_H

    #include <stdbool.h>
    #include <stddef.h>
    typedef struct Map Map;
    
    typedef size_t (*MapHash)(const void*);
    typedef int (*MapCompare)(const void*, const void*);
    typedef void (*MapFree)(void*);

    Map* mapCreate(size_t initialCapacity, MapHash hash, MapCompare compare, MapFree freeKey, MapFree freeValue);

    // Convenience function for string keys (uses djb2 hash and strcmp)
    Map* mapCreateStringMap(size_t initialCapacity);

    void mapDestroy(Map* map);

    bool mapPut(Map* map, void* key, void* value);
    void* mapGet(Map* map, void* key);
    bool mapRemove(Map* map, void* key);
    bool mapContains(Map* map, void* key);
    size_t mapSize(Map* map);
    void mapClear(Map* map);
    bool mapResize(Map* map, size_t newCapacity);

#endif