# CMap Library

A simple, hash-based map (dictionary) implementation for C, designed for efficient key-value storage. This library provides customizable hashing, comparison, and memory management for flexibility.

## Features

- **Hash-Based Storage**: Average O(1) time complexity for operations using separate chaining.
- **Customizable Functions**: Provide your own hash, compare, and free functions for keys and values.
- **Memory Management**: Optional automatic freeing of keys and values on removal/destruction.
- **Thread-Unsafe**: Designed for single-threaded use.
- **Resizing**: Dynamic capacity expansion.

## Limitations

- **Memory Responsibility**: If using automatic freeing, ensure keys/values are heap-allocated. Otherwise, provide NULL for free functions.
- **No Built-in Types**: Requires custom functions for non-pointer types.
- **Collision Handling**: Uses chaining; may degrade with poor hash functions.
- **Not Thread-Safe**: Concurrent access may cause issues.

## API Reference

### Types

- `Map`: Opaque struct representing the hash map.
- `MapHash`: Function to hash a key: `size_t (*)(const void* key)`.
- `MapCompare`: Function to compare keys: `int (*)(const void* key1, const void* key2)` (return 0 if equal).
- `MapFree`: Function to free a pointer: `void (*)(void* ptr)`.

### Functions

#### `Map* mapCreate(size_t initialCapacity, MapHash hash, MapCompare compare, MapFree freeKey, MapFree freeValue)`
Creates a new hash map with custom functions.
- **Parameters**:
  - `initialCapacity`: Initial number of buckets (recommended prime number).
  - `hash`: Hash function for keys.
  - `compare`: Comparison function for keys.
  - `freeKey`: Function to free keys, or NULL to skip.
  - `freeValue`: Function to free values, or NULL to skip.
- **Returns**: Pointer to the new `Map`, or `NULL` on failure.
- **Notes**: Capacity must be > 0.

#### `Map* mapCreateStringMap(size_t initialCapacity)`
Convenience function for string keys and values.
- **Uses**: djb2 hash, strcmp for comparison, free for both keys and values.
- **Note**: Keys and values must be heap-allocated strings.

#### `void mapDestroy(Map* map)`
Destroys the map and frees all memory.
- **Parameters**:
  - `map`: The map to destroy.
- **Notes**: Calls free functions on all remaining keys/values.

#### `bool mapPut(Map* map, void* key, void* value)`
Inserts or updates a key-value pair.
- **Parameters**:
  - `map`: The map.
  - `key`: The key.
  - `value`: The value.
- **Returns**: `true` on success, `false` on failure (e.g., NULL map/key).
- **Notes**: If key exists, old value is freed if freeValue is set.

#### `void* mapGet(Map* map, void* key)`
Retrieves the value for a key.
- **Parameters**:
  - `map`: The map.
  - `key`: The key.
- **Returns**: The value, or `NULL` if not found.

#### `bool mapRemove(Map* map, void* key)`
Removes a key-value pair.
- **Parameters**:
  - `map`: The map.
  - `key`: The key.
- **Returns**: `true` if removed, `false` otherwise.
- **Notes**: Frees key and value if free functions are set.

#### `bool mapContains(Map* map, void* key)`
Checks if a key exists.
- **Parameters**:
  - `map`: The map.
  - `key`: The key.
- **Returns**: `true` if exists.

#### `size_t mapSize(Map* map)`
Returns the number of entries.
- **Parameters**:
  - `map`: The map.
- **Returns**: Current size.

#### `void mapClear(Map* map)`
Removes all entries.
- **Parameters**:
  - `map`: The map.
- **Notes**: Frees keys/values if free functions are set.

#### `bool mapResize(Map* map, size_t newCapacity)`
Resizes the map's capacity.
- **Parameters**:
  - `map`: The map.
  - `newCapacity`: New bucket count (> current size).
- **Returns**: `true` on success.
- **Notes**: Rehashes all entries.

## Example Usage

```c
#include <map.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Create a map for string keys and values
    Map* map = mapCreate(10, "str", "str");
    if (!map) {
        fprintf(stderr, "Failed to create map\n");
        return 1;
    }

    // Add entries (note: using string literals, which are safe)
    mapPut(map, "key1", "value1");
    mapPut(map, "key2", "value2");

    // Retrieve a value
    char* val = (char*)mapGet(map, "key1");
    if (val) {
        printf("Retrieved: %s\n", val);
    }

    // Check existence
    if (mapContains(map, "key2")) {
        printf("Key exists\n");
    }

    // Remove an entry
    mapRemove(map, "key1");

    // Get size
    printf("Size: %zu\n", mapSize(map));

    // Clear the map
    mapClear(map);

    // Destroy the map
    mapDestroy(map);

    return 0;
}
```

For dynamic strings, manage memory carefully:

```c
char* key = strdup("dynamic_key");
char* value = strdup("dynamic_value");
mapPut(map, key, value);
// ... use ...
free(key);
free(value);
```

## Building and Installation

### System-Wide Installation (Recommended for CMake Projects)

Build and install the library system-wide, then use it in CMake projects via `find_package`:

```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"  # or your preferred generator
mingw32-make
mingw32-make install  # installs to /usr/local or equivalent
```

In your project's `CMakeLists.txt`:
```cmake
find_package(cmap REQUIRED)
target_link_libraries(your_target cmap::cmap)
```

This installs:
- Headers to `include/map.h`
- Static library to `lib/libcmap.a`
- CMake config files to `lib/cmake/cmap/`

### As a CMake Subdirectory

Add the library as a subdirectory in your project:

```cmake
add_subdirectory(path/to/cmap)
target_link_libraries(your_target cmap)
target_include_directories(your_target PRIVATE path/to/cmap/include)
```

### Manual Integration

Copy the files manually:
- Include `include/map.h` in your project
- Link with `build/libcmap.a` (after building)
- Ensure the include path points to the header location

### Chocolatey Package (Windows)

If on Windows with Chocolatey:
```bash
choco install cmap
```

This installs to `C:\Program Files\cmap\` with headers and library.

### Building the Example

To build the included example:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

The library is designed to be linked statically, so distribute the header and compiled library with your project or use one of the installation methods above.

## Version

1.0.0a - Initial hash-based implementation.