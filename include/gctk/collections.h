#ifndef GCTK_COLLECTIONS_H
#define GCTK_COLLECTIONS_H

#include "gctk/common.h"
#include "gctk/bithelper.h"
#include "gctk/str.h"

#if !defined(NDEBUG) || defined(GCTK_ALLOW_MEMORY_LOG)
GCTK_API void GctkReportMemoryLeakCount();
GCTK_API void GctkCountAlloc(void* ptr, size_t size);
GCTK_API void GctkCountRealloc(void* ptr_old, void* ptr_new, size_t old_size, size_t new_size);
GCTK_API void GctkCountDealloc(void* ptr);
#endif

typedef void* (*GctkAllocatorFn)(size_t count, size_t item_size);
typedef void* (*GctkReallocatorFn)(void** ptr, size_t original_count, size_t new_count, size_t item_size);
typedef void (*GctkFreeFn)(void** ptr);

typedef struct Allocator {
	size_t item_size;
	GctkAllocatorFn allocate;
	GctkReallocatorFn reallocate;
	GctkFreeFn free;
} Allocator;

#define ALLOCATOR(__item_size, __alloc_fn, __realloc_fn, __free) \
((Allocator){ .item_size = (__item_size), .allocate = (__alloc_fn), .reallocate = (__realloc_fn), .free = (__free) })

#define NULL_ALLOCATOR ALLOCATOR(0, NULL, NULL, NULL)

GCTK_API void* GctkAlloc(const Allocator* allocator, size_t count);
GCTK_API void* GctkRealloc(const Allocator* allocator, void** ptr, size_t original_count, size_t new_count);
GCTK_API void  GctkFree(const Allocator* allocator, void** ptr);

GCTK_API const Allocator* GctkGetDefaultAllocator();
GCTK_API Allocator GctkCreateDefaultAllocator(size_t item_size);
GCTK_API const Allocator* GctkSetupDefaultAllocator(Allocator* target, size_t item_size);

typedef struct Vector {
	void* data;
	size_t capacity, count;
	const Allocator* allocator;
	void (*item_remove_callback)(void* ptr, size_t size);
} Vector;

GCTK_API bool GctkVectorAlloc(Vector* vec, size_t capacity, const Allocator* allocator);
GCTK_API void GctkVectorFree (Vector* vec);

GCTK_API bool GctkVectorAddItem   (Vector* vec, const void* data, size_t count);
GCTK_API bool GctkVectorInsertItem(Vector* vec, size_t idx, const void* data, size_t count);
GCTK_API bool GctkVectorRemoveItem(Vector* vec, size_t idx);
GCTK_API bool GctkVectorPopBack   (Vector* vec);
GCTK_API bool GctkVectorPopFront  (Vector* vec);
GCTK_API bool GctkVectorReserve   (Vector* vec, size_t new_capacity);
GCTK_API bool GctkVectorClear     (Vector* vec);

GCTK_API void* GctkVectorGet           (Vector* vec, size_t idx);
GCTK_API const void* GctkVectorGetConst(const Vector* vec, size_t idx);

#define GctkVectorCastToArray(T, __vecptr__) ((T*)(__vecptr__)->data)

GCTK_API bool GctkVectorWrite_u8 (Vector* vec, uint8_t value);
GCTK_API bool GctkVectorWrite_u16(Vector* vec, uint16_t value);
GCTK_API bool GctkVectorWrite_u32(Vector* vec, uint32_t value);
GCTK_API bool GctkVectorWrite_u64(Vector* vec, uint64_t value);
GCTK_API bool GctkVectorWrite_i8 (Vector* vec, int8_t value);
GCTK_API bool GctkVectorWrite_i16(Vector* vec, int16_t value);
GCTK_API bool GctkVectorWrite_i32(Vector* vec, int32_t value);
GCTK_API bool GctkVectorWrite_i64(Vector* vec, int64_t value);
GCTK_API bool GctkVectorWrite_f32(Vector* vec, float value);
GCTK_API bool GctkVectorWrite_f64(Vector* vec, double value);
GCTK_API bool GctkVectorWrite_str(Vector* vec, const char* value);

typedef struct HashMapPair {
	hash_t hash;
	void* data;
} HashMapPair;

typedef struct HashMap {
	HashMapPair* pairs;
	size_t capacity, count;
	const Allocator* item_allocator;
} HashMap;

GCTK_API bool GctkHashMapAlloc(HashMap* map, size_t capacity, const Allocator* item_allocator);
GCTK_API bool GctkHashMapFree (HashMap* map);

GCTK_API ssize_t GctkHashMapFind      (const HashMap* map, const char* key);
GCTK_API ssize_t GctkHashMapFindByHash(const HashMap* map, hash_t hash);

GCTK_API HashMapPair* GctkHashMapGet                 (HashMap* map, const char* key);
GCTK_API const HashMapPair* GctkHashMapGetConst      (const HashMap* map, const char* key);
GCTK_API HashMapPair* GctkHashMapGetByHash           (HashMap* map, hash_t hash);
GCTK_API const HashMapPair* GctkHashMapGetByHashConst(const HashMap* map, hash_t hash);

GCTK_API bool GctkHashMapAdd         (HashMap* map, const char* key, const void* data);
GCTK_API bool GctkHashMapAddWithHash (HashMap* map, hash_t hash, const void* data);
GCTK_API bool GctkHashMapRemove      (HashMap* map, const char* key);
GCTK_API bool GctkHashMapRemoveByHash(HashMap* map, hash_t hash);
GCTK_API bool GctkHashMapReserve     (HashMap* map, size_t new_capacity);
GCTK_API bool GctkHashMapClear       (HashMap* map);

typedef struct BinaryWriter {
	union {
		FILE* file;
		Vector* buffer;
	};
	bool is_file;
	Endianness endianness;
} BinaryWriter;

GCTK_API BinaryWriter GctkBinaryWriterNewFromFile(FILE* f, Endianness endianness);
GCTK_API BinaryWriter GctkBinaryWriterNewFromVector(Vector* vector, Endianness endianness);
GCTK_API void GctkBinaryWriterClose(BinaryWriter* writer);

GCTK_API bool GctkBinaryWriterAppend_u8 (BinaryWriter* writer, uint8_t value);
GCTK_API bool GctkBinaryWriterAppend_u16(BinaryWriter* writer, uint16_t value);
GCTK_API bool GctkBinaryWriterAppend_u32(BinaryWriter* writer, uint32_t value);
GCTK_API bool GctkBinaryWriterAppend_u64(BinaryWriter* writer, uint64_t value);
GCTK_API bool GctkBinaryWriterAppend_i8 (BinaryWriter* writer, int8_t value);
GCTK_API bool GctkBinaryWriterAppend_i16(BinaryWriter* writer, int16_t value);
GCTK_API bool GctkBinaryWriterAppend_i32(BinaryWriter* writer, int32_t value);
GCTK_API bool GctkBinaryWriterAppend_i64(BinaryWriter* writer, int64_t value);
GCTK_API bool GctkBinaryWriterAppend_f32(BinaryWriter* writer, float value);
GCTK_API bool GctkBinaryWriterAppend_f64(BinaryWriter* writer, double value);
GCTK_API bool GctkBinaryWriterAppend_str(BinaryWriter* writer, const char* value);
GCTK_API bool GctkBinaryWriterAppendBytes(BinaryWriter* writer, const uint8_t* value, size_t size);

#define GctkBinaryWriterAppend(__writer__, __value__) _Generic((__value__),\
	uint8_t:  GctkBinaryWriterAppend_u8(__writer__, __value__),\
	uint16_t: GctkBinaryWriterAppend_u16(__writer__, __value__),\
	uint32_t: GctkBinaryWriterAppend_u32(__writer__, __value__),\
	uint64_t: GctkBinaryWriterAppend_u64(__writer__, __value__),\
	int8_t:  GctkBinaryWriterAppend_i8(__writer__, __value__),\
	int16_t: GctkBinaryWriterAppend_i16(__writer__, __value__),\
	int32_t: GctkBinaryWriterAppend_i32(__writer__, __value__),\
	int64_t: GctkBinaryWriterAppend_i64(__writer__, __value__),\
	float:  GctkBinaryWriterAppend_f32(__writer__, __value__),\
	double: GctkBinaryWriterAppend_f64(__writer__, __value__),\
	char*:  GctkBinaryWriterAppend_str(__writer__, __value__),\
	const char*: GctkBinaryWriterAppend_str(__writer__, __value__)\
)

#endif