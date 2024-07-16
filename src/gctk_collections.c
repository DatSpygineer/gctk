#include <gctk/filesys.h>

#include "gctk/collections.h"
#include "gctk/debug.h"

void* GctkMallocImpl(size_t count, size_t item_size);
void* GctkReallocImpl(void** ptr, size_t original_count, size_t new_count, size_t item_size);
void GctkFreeImpl(void** ptr);

#if !defined(NDEBUG) || defined(GCTK_ALLOW_MEMORY_LOG)
typedef struct GctkMemoryCounter {
	void* ptr;
	size_t count;
} GctkMemoryCounter;

static GctkMemoryCounter GCTK_MEMORY_COUNTER[1024] = { 0 };
static size_t GCTK_MEMORY_COUNTER_COUNT = 0;

void GctkReportMemoryLeakCount() {
	bool leak_detected = false;
	for (size_t i = 0; i < GCTK_MEMORY_COUNTER_COUNT; i++) {
		if (GCTK_MEMORY_COUNTER[i].ptr != NULL && GCTK_MEMORY_COUNTER[i].count > 0) {
			GctkLogError(GCTK_ERROR_MEMORY_LEAK, "Memory leak detected for pointer %p", GCTK_MEMORY_COUNTER[i].ptr);
			leak_detected = true;
		}
	}

	if (!leak_detected) {
		GctkLog("No memory leak detected!");
	}
}

void GctkCountAlloc(void* ptr, size_t size) {
	bool assigned = false;
	for (size_t i = 0; i < GCTK_MEMORY_COUNTER_COUNT; i++) {
		if (GCTK_MEMORY_COUNTER[i].ptr == ptr) {
			GCTK_MEMORY_COUNTER[i].count++;
			assigned = true;
			break;
		}
	}
	if (!assigned) {
		GCTK_MEMORY_COUNTER[GCTK_MEMORY_COUNTER_COUNT++] = (GctkMemoryCounter) {
				.ptr = ptr,
				.count = 1
		};
	}
	GctkLog("[ Allocated {%d} bytes at address %p ]", size, ptr);
}
void GctkCountRealloc(void* ptr_old, void* ptr_new, size_t old_size, size_t new_size) {
	for (size_t i = 0; i < GCTK_MEMORY_COUNTER_COUNT; i++) {
		if (GCTK_MEMORY_COUNTER[i].ptr == ptr_old) {
			GCTK_MEMORY_COUNTER[i].count--;
			break;
		}
	}
	bool assigned = false;
	for (size_t i = 0; i < GCTK_MEMORY_COUNTER_COUNT; i++) {
		if (GCTK_MEMORY_COUNTER[i].ptr == ptr_new) {
			GCTK_MEMORY_COUNTER[i].count++;
			assigned = true;
			break;
		}
	}
	if (!assigned) {
		GCTK_MEMORY_COUNTER[GCTK_MEMORY_COUNTER_COUNT++] = (GctkMemoryCounter) {
			.ptr = ptr_new,
			.count = 1
		};
	}
	GctkLog("[ Reallocated memory %p to %p (old size: %d, new size: %d) ]", ptr_old, ptr_new, old_size, new_size);
}
void GctkCountDealloc(void* ptr) {
	for (size_t i = 0; i < GCTK_MEMORY_COUNTER_COUNT; i++) {
		if (GCTK_MEMORY_COUNTER[i].ptr == ptr) {
			GCTK_MEMORY_COUNTER[i].count--;
			break;
		}
	}
	GctkLog("[ Deallocated memory %p ]", ptr);
}
#endif

static Allocator GCTK_DEFAULT_ALLOCATOR = ALLOCATOR(sizeof(uint8_t), &GctkMallocImpl, &GctkReallocImpl, &GctkFreeImpl);
static Allocator GCTK_HASHMAP_PAIR_ALLOCATOR = ALLOCATOR(sizeof(HashMapPair), &GctkMallocImpl, &GctkReallocImpl, &GctkFreeImpl);

void* GctkMallocImpl(size_t count, size_t item_size) {
	void* ptr = calloc(count, item_size);
#if !defined(NDEBUG) || defined(GCTK_ALLOW_MEMORY_LOG)
	if (ptr != NULL) {
		GctkCountAlloc(ptr, count * item_size);
	} else {
#else
	if (ptr == NULL) {
#endif
		GctkLogError(GCTK_ERROR_OUT_OF_MEMORY, "Failed to allocate {%d} bytes!", count * item_size);
	}
	return ptr;
}
void* GctkReallocImpl(void** ptr, size_t original_count, size_t new_count, size_t item_size) {
	if (ptr == NULL) {
		return NULL;
	}

	void* new_ptr = calloc(new_count, item_size);
	if (new_ptr == NULL) {
		GctkLogError(GCTK_ERROR_OUT_OF_MEMORY, "Failed to reallocate %p: Could not allocate {%d} bytes!", ptr,
					 new_count * item_size);
		return NULL;
	}

	memset(new_ptr, 0, new_count * item_size);
	if (*ptr != NULL) {
		const size_t copy_count = original_count < new_count ? original_count : new_count;
		memcpy(new_ptr, *ptr, copy_count * item_size);
#if !defined(NDEBUG) || defined(GCTK_ALLOW_MEMORY_LOG)
		GctkCountRealloc(*ptr, new_ptr, original_count * item_size, new_count * item_size);
#endif
	} else {
#if !defined(NDEBUG) || defined(GCTK_ALLOW_MEMORY_LOG)
		GctkCountAlloc(new_ptr, original_count * item_size);
		GctkLog("[ Allocated {%d} bytes at address %p ]", new_count * item_size, *ptr);
#endif
	}

	*ptr = new_ptr;
	return *ptr;
}
void GctkFreeImpl(void** ptr) {
	if (ptr) {
		free(*ptr);
#if !defined(NDEBUG) || defined(GCTK_ALLOW_MEMORY_LOG)
		GctkCountDealloc(*ptr);
#endif
		*ptr = NULL;
	}
}

void* GctkAlloc(const Allocator* allocator, size_t count) {
	if (allocator == NULL) {
		GctkLogError(GCTK_ERROR_NULL_PTR, "Allocator is null!");
		return NULL;
	}
	if (allocator->allocate == NULL) {
		GctkLogError(GCTK_ERROR_NULL_PTR, "Allocator function \"allocate\" is null!");
		return NULL;
	}
	if (allocator->item_size == 0) {
		GctkLogError(GCTK_ERROR_OUT_OF_RANGE, "Allocator item_size must be more then 0!");
		return NULL;
	}

	return allocator->allocate(count, allocator->item_size);
}
void* GctkRealloc(const Allocator* allocator, void** ptr, size_t original_count, size_t new_count) {
	if (allocator == NULL) {
		GctkLogError(GCTK_ERROR_NULL_PTR, "Allocator is null!");
		return NULL;
	}
	if (allocator->reallocate == NULL) {
		GctkLogError(GCTK_ERROR_NULL_PTR, "Allocator function \"reallocate\" is null!");
		return NULL;
	}
	if (allocator->item_size == 0) {
		GctkLogError(GCTK_ERROR_OUT_OF_RANGE, "Allocator item_size must be more then 0!");
		return NULL;
	}

	return allocator->reallocate(ptr, original_count, new_count, allocator->item_size);
}
void  GctkFree(const Allocator* allocator, void** ptr) {
	if (allocator == NULL) {
		GctkLogError(GCTK_ERROR_NULL_PTR, "Allocator is null!");
		return;
	}
	if (allocator->free == NULL) {
		GctkLogError(GCTK_ERROR_NULL_PTR, "Allocator function \"free\" is null!");
		return;
	}

	allocator->free(ptr);
}

const Allocator* GctkGetDefaultAllocator() {
	return &GCTK_DEFAULT_ALLOCATOR;
}
Allocator GctkCreateDefaultAllocator(size_t item_size) {
	return ALLOCATOR(item_size, &GctkMallocImpl, &GctkReallocImpl, &GctkFreeImpl);
}

bool GctkVectorAlloc(Vector* vec, size_t capacity, const Allocator* allocator) {
	if (allocator == NULL) {
		GctkLogError(GCTK_ERROR_NULL_PTR, "Allocator is null!");
		return false;
	}

	*vec = (Vector) {
		.data = NULL,
		.capacity = capacity,
		.count = 0,
		.allocator = allocator
	};

	if (capacity > 0) {
		vec->data = GctkAlloc(allocator, capacity);
		if (vec->data == NULL) {
			return false;
		}
	}

	return true;
}
void GctkVectorFree(Vector* vec) {
	if (vec && vec->data) {
		GctkFree(vec->allocator, &vec->data);
	}
}

bool GctkVectorAddItem(Vector* vec, const void* data, size_t count) {
	if (vec == NULL) {
		return false;
	}

	if (vec->capacity <= vec->count + count) {
		GctkVectorReserve(vec, vec->count + count);
	}

	memcpy(((uint8_t*)vec->data) + (vec->count * vec->allocator->item_size), data, count * vec->allocator->item_size);
	vec->count += count;
	return true;
}
bool GctkVectorInsertItem(Vector* vec, size_t idx, const void* data, size_t count) {
	if (vec == NULL) {
		return false;
	}

	if (vec->capacity <= vec->count + count) {
		GctkVectorReserve(vec, vec->count + count);
	}
	uint8_t* src = ((uint8_t*)vec->data) + (idx * vec->allocator->item_size);
	memmove(src + (count * vec->allocator->item_size), src, (vec->count - idx) * vec->allocator->item_size);
	memcpy(src, data, count * vec->allocator->item_size);

	return true;
}
bool GctkVectorRemoveItem(Vector* vec, size_t idx) {
	if (vec == NULL) {
		return false;
	}

	if (idx == 0) {
		return GctkVectorPopFront(vec);
	} else if (idx == vec->count - 1) {
		return GctkVectorPopBack(vec);
	}

	void* trg = vec->data + (idx * vec->allocator->item_size);
	void* src = trg + vec->allocator->item_size;

	if (vec->item_remove_callback != NULL) {
		vec->item_remove_callback(trg, vec->allocator->item_size);
	}
	memmove(trg, src, (vec->count - idx) * vec->allocator->item_size);
	vec->count--;
	memset(vec->data + ((vec->count + 1) * vec->allocator->item_size), 0, vec->allocator->item_size);

	return true;
}
bool GctkVectorPopBack(Vector* vec) {
	if (vec == NULL) {
		return false;
	}

	if (vec->item_remove_callback != NULL) {
		vec->item_remove_callback(vec->data + ((vec->count - 1) * vec->allocator->item_size), vec->allocator->item_size);
	}
	memset(((uint8_t*)vec->data) + ((vec->count - 1) * vec->allocator->item_size), 0, vec->allocator->item_size);
	vec->count--;
	return true;
}
bool GctkVectorPopFront(Vector* vec) {
	if (vec == NULL) {
		return false;
	}

	if (vec->item_remove_callback != NULL) {
		vec->item_remove_callback(vec->data, vec->allocator->item_size);
	}
	memmove(vec->data, ((uint8_t*)vec->data) + (vec->count * vec->allocator->item_size), (vec->count - 2) * vec->allocator->item_size);
	return true;
}
bool GctkVectorReserve(Vector* vec, size_t new_capacity) {
	if (vec == NULL) {
		return false;
	}

	if (vec->data == NULL) {
		vec->data = GctkAlloc(vec->allocator, new_capacity);
	} else {
		if (new_capacity < vec->count) {
			new_capacity = vec->count; // Limit to current count
		}

		if (new_capacity != vec->capacity) {
			vec->data = GctkRealloc(vec->allocator, &vec->data, vec->capacity, new_capacity);
		}
	}
	vec->capacity = new_capacity;
	return true;
}
bool GctkVectorClear(Vector* vec) {
	if (vec == NULL) {
		return false;
	}

	if (vec->item_remove_callback != NULL) {
		for (size_t i = 0; i < vec->count; i++) {
			vec->item_remove_callback(vec->data + (i * vec->allocator->item_size), vec->allocator->item_size);
		}
	}
	memset(vec->data, 0, vec->count * vec->allocator->item_size);
	vec->count = 0;
	return true;
}

void* GctkVectorGet(Vector* vec, size_t idx) {
	if (vec == NULL || idx >= vec->count) {
		return NULL;
	}
	return vec->data + (idx * vec->allocator->item_size);
}
const void* GctkVectorGetConst(const Vector* vec, size_t idx) {
	if (vec == NULL || idx >= vec->count) {
		return NULL;
	}
	return vec->data + (idx * vec->allocator->item_size);
}

bool GctkVectorWrite_u8 (Vector* vec, uint8_t value) {
	return GctkVectorAddItem(vec, &value, sizeof(value));
}
bool GctkVectorWrite_u16(Vector* vec, uint16_t value) {
	return GctkVectorAddItem(vec, &value, sizeof(value));
}
bool GctkVectorWrite_u32(Vector* vec, uint32_t value) {
	return GctkVectorAddItem(vec, &value, sizeof(value));
}
bool GctkVectorWrite_u64(Vector* vec, uint64_t value) {
	return GctkVectorAddItem(vec, &value, sizeof(value));
}
bool GctkVectorWrite_i8 (Vector* vec, int8_t value) {
	return GctkVectorAddItem(vec, &value, sizeof(value));
}
bool GctkVectorWrite_i16(Vector* vec, int16_t value) {
	return GctkVectorAddItem(vec, &value, sizeof(value));
}
bool GctkVectorWrite_i32(Vector* vec, int32_t value) {
	return GctkVectorAddItem(vec, &value, sizeof(value));
}
bool GctkVectorWrite_i64(Vector* vec, int64_t value) {
	return GctkVectorAddItem(vec, &value, sizeof(value));
}
bool GctkVectorWrite_f32(Vector* vec, float value) {
	return GctkVectorAddItem(vec, &value, sizeof(value));
}
bool GctkVectorWrite_f64(Vector* vec, double value) {
	return GctkVectorAddItem(vec, &value, sizeof(value));
}
bool GctkVectorWrite_str(Vector* vec, const char* value) {
	return GctkVectorAddItem(vec, &value, sizeof(value));
}

bool GctkHashMapAlloc(HashMap* map, size_t capacity, const Allocator* item_allocator) {
	if (map == NULL) {
		return false;
	}

	void* p = GctkAlloc(&GCTK_HASHMAP_PAIR_ALLOCATOR, capacity);
	if (p == NULL) {
		return false;
	}

	*map = (HashMap) {
		.pairs = p,
		.capacity = capacity,
		.count = 0,
		.item_allocator = item_allocator
	};

	return true;
}
bool GctkHashMapFree(HashMap* map) {
	if (map == NULL) {
		return false;
	}

	for (size_t i = 0; i < map->count; i++) {
		GctkFree(map->item_allocator, &map->pairs[i].data);
	}
	GctkFree(&GCTK_HASHMAP_PAIR_ALLOCATOR, (void**)&map->pairs);
	memset(map, 0, sizeof(HashMap));

	return true;
}

ssize_t GctkHashMapFind(const HashMap* map, const char* key) {
	return GctkHashMapFindByHash(map, GctkStrHash(key));
}
ssize_t GctkHashMapFindByHash(const HashMap* map, hash_t hash) {
	if (map == NULL) {
		return -1;
	}

	for (ssize_t i = 0; i < map->count; i++) {
		if (map->pairs[i].hash == hash) {
			return i;
		}
	}
	return -1;
}

HashMapPair* GctkHashMapGet(HashMap* map, const char* key) {
	return GctkHashMapGetByHash(map, GctkStrHash(key));
}
const HashMapPair* GctkHashMapGetConst(const HashMap* map, const char* key) {
	return GctkHashMapGetByHashConst(map, GctkStrHash(key));
}

HashMapPair* GctkHashMapGetByHash(HashMap* map, hash_t hash) {
	ssize_t i = GctkHashMapFindByHash(map, hash);
	if (i < 0) {
		return NULL;
	}
	return map->pairs + i;
}
const HashMapPair* GctkHashMapGetByHashConst(const HashMap* map, hash_t hash) {
	ssize_t i = GctkHashMapFindByHash(map, hash);
	if (i < 0) {
		return NULL;
	}
	return map->pairs + i;
}

bool GctkHashMapAdd(HashMap* map, const char* key, const void* data) {
	return GctkHashMapAddWithHash(map, GctkStrHash(key), data);
}
bool GctkHashMapAddWithHash(HashMap* map, hash_t hash, const void* data) {
	if (map == NULL) {
		return false;
	}

	void* pair_data = GctkAlloc(map->item_allocator, 1);
	if (pair_data == NULL) {
		return false;
	}
	memcpy(pair_data, data, map->item_allocator->item_size);

	if (map->capacity <= map->count + 1) {
		if (!GctkHashMapReserve(map, map->count + 1)) {
			GctkFree(map->item_allocator, pair_data);
			return false;
		}
	}

	map->pairs[map->count++] = (HashMapPair){
		.hash = hash,
		.data = pair_data
	};

	return true;
}
bool GctkHashMapRemove(HashMap* map, const char* key) {
	return GctkHashMapRemoveByHash(map, GctkStrHash(key));
}
bool GctkHashMapRemoveByHash(HashMap* map, hash_t hash) {
	if (map == NULL) {
		return false;
	}
	ssize_t idx;
	if ((idx = GctkHashMapFindByHash(map, hash)) < 0) {
		return false;
	}

	GctkFree(map->item_allocator, &map->pairs[idx].data);

	for (size_t i = idx; i < map->count; i++) {
		if (i + 1 < map->count) {
			map->pairs[i] = map->pairs[i + 1];
		} else {
			memset(&map->pairs[i], 0, sizeof(HashMapPair));
		}
	}
	map->count--;

	return true;
}
bool GctkHashMapReserve(HashMap* map, size_t new_capacity) {
	if (map == NULL) {
		return false;
	}

	if (map->pairs == NULL) {
		map->pairs = GctkAlloc(&GCTK_HASHMAP_PAIR_ALLOCATOR, new_capacity);
		if (map->pairs == NULL) {
			return false;
		}
	} else {
		if (new_capacity < map->count) {
			new_capacity = map->count;
		}
		if (map->capacity != new_capacity) {
			map->pairs = GctkRealloc(&GCTK_HASHMAP_PAIR_ALLOCATOR, (void**)&map->pairs, map->capacity, new_capacity);
			if (map->pairs == NULL) {
				return false;
			}
		}
	}
	map->capacity = new_capacity;
	return true;
}
bool GctkHashMapClear(HashMap* map) {
	if (map == NULL) {
		return false;
	}

	for (size_t i = 0; i < map->count; i++) {
		GctkFree(map->item_allocator, &map->pairs[i].data);
	}
	memset(map->pairs, 0, sizeof(HashMapPair) * map->count);

	return true;
}

BinaryWriter GctkBinaryWriterNewFromFile(FILE* f, Endianness endianness) {
	return (BinaryWriter) {
		.file =  f,
		.is_file = true,
		.endianness = endianness
	};
}
BinaryWriter GctkBinaryWriterNewFromVector(Vector* vector, Endianness endianness) {
	return (BinaryWriter) {
		.buffer = vector,
		.is_file = false,
		.endianness = endianness
	};
}
void GctkBinaryWriterClose(BinaryWriter* writer) {
	if (writer) {
		if (writer->is_file) {
			fflush(writer->file);
			fclose(writer->file);
		} else {
			GctkVectorFree(writer->buffer);
		}
	}
}

bool GctkBinaryWriterAppend_u8 (BinaryWriter* writer, uint8_t value) {
	if (writer == NULL) return false;
	if (writer->is_file) {
		return GctkFileWrite_u8(writer->file, value) > 0;
	}
	return GctkVectorWrite_u8(writer->buffer, value);
}
bool GctkBinaryWriterAppend_u16(BinaryWriter* writer, uint16_t value) {
	if (writer == NULL) return false;

	if (GCTK_ENDIANNESS != writer->endianness) {
		value = GctkReverseU16(value);
	}

	if (writer->is_file) {
		return GctkFileWrite_u16(writer->file, value) > 0;
	}
	return GctkVectorWrite_u16(writer->buffer, value);
}
bool GctkBinaryWriterAppend_u32(BinaryWriter* writer, uint32_t value) {
	if (writer == NULL) return false;

	if (GCTK_ENDIANNESS != writer->endianness) {
		value = GctkReverseU32(value);
	}

	if (writer->is_file) {
		return GctkFileWrite_u32(writer->file, value) > 0;
	}
	return GctkVectorWrite_u32(writer->buffer, value);
}
bool GctkBinaryWriterAppend_u64(BinaryWriter* writer, uint64_t value) {
	if (writer == NULL) return false;

	if (GCTK_ENDIANNESS != writer->endianness) {
		value = GctkReverseU64(value);
	}

	if (writer->is_file) {
		return GctkFileWrite_u64(writer->file, value) > 0;
	}
	return GctkVectorWrite_u64(writer->buffer, value);
}
bool GctkBinaryWriterAppend_i8 (BinaryWriter* writer, int8_t value) {
	if (writer == NULL) return false;
	if (writer->is_file) {
		return GctkFileWrite_i8(writer->file, value) > 0;
	}
	return GctkVectorWrite_i8(writer->buffer, value);
}
bool GctkBinaryWriterAppend_i16(BinaryWriter* writer, int16_t value) {
	if (writer == NULL) return false;

	if (GCTK_ENDIANNESS != writer->endianness) {
		value = GctkReverseI16(value);
	}

	if (writer->is_file) {
		return GctkFileWrite_i16(writer->file, value) > 0;
	}
	return GctkVectorWrite_i16(writer->buffer, value);
}
bool GctkBinaryWriterAppend_i32(BinaryWriter* writer, int32_t value) {
	if (writer == NULL) return false;

	if (GCTK_ENDIANNESS != writer->endianness) {
		value = GctkReverseI32(value);
	}

	if (writer->is_file) {
		return GctkFileWrite_i32(writer->file, value) > 0;
	}
	return GctkVectorWrite_i32(writer->buffer, value);
}
bool GctkBinaryWriterAppend_i64(BinaryWriter* writer, int64_t value) {
	if (writer == NULL) return false;

	if (GCTK_ENDIANNESS != writer->endianness) {
		value = GctkReverseI64(value);
	}

	if (writer->is_file) {
		return GctkFileWrite_i64(writer->file, value) > 0;
	}
	return GctkVectorWrite_i64(writer->buffer, value);
}
bool GctkBinaryWriterAppend_f32(BinaryWriter* writer, float value) {
	if (writer == NULL) return false;
	if (writer->is_file) {
		return GctkFileWrite_f32(writer->file, value) > 0;
	}
	return GctkVectorWrite_f32(writer->buffer, value);
}
bool GctkBinaryWriterAppend_f64(BinaryWriter* writer, double value) {
	if (writer == NULL) return false;
	if (writer->is_file) {
		return GctkFileWrite_f64(writer->file, value) > 0;
	}
	return GctkVectorWrite_f64(writer->buffer, value);
}
bool GctkBinaryWriterAppend_str(BinaryWriter* writer, const char* value) {
	if (writer == NULL) return false;
	if (writer->is_file) {
		return GctkFileWrite_str(writer->file, value) > 0;
	}
	return GctkVectorWrite_str(writer->buffer, value);
}
bool GctkBinaryWriterAppendBytes(BinaryWriter* writer, const uint8_t* value, size_t size) {
	if (writer == NULL) return false;
	if (writer->is_file) {
		return GctkFileWrite(writer->file, value, size, sizeof(uint8_t)) < size;
	}
	return GctkVectorAddItem(writer->buffer, value, size);
}