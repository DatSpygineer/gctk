#include "gctk/str.h"

#define GctkMax(__x__, __max__) ((__x__) > (__max__) ? (__max__) : (__x__))

char* GctkStrCpy(char* trg, const char* source, size_t max_size) {
	size_t len = GctkMax(strlen(source), max_size);
	char* p = strncpy(trg, source, len);
	if (p != NULL) {
		trg[len] = '\0';
	}
	return p;
}
char* GctkStrCat(char* trg, const char* lhs, const char* rhs, size_t max_size) {
	if (GctkStrCpy(trg, lhs, max_size) == NULL) {
		return NULL;
	}
	const size_t lhs_size = strlen(lhs);
	return lhs_size < max_size ? GctkStrCpy(trg + lhs_size, rhs, max_size - lhs_size) : trg;
}
char* GctkStrAppend(char* trg, const char* src, size_t max_size) {
	const size_t trg_size = strlen(trg);
	if (strlen(trg) < max_size) {
		return GctkStrCpy(trg + trg_size, src, max_size - trg_size);
	}
	return NULL;
}

bool GctkStrStartWith(const char* str, char start_with) {
	const size_t len = str ? strlen(str) : 0;
	return len > 0 && str[0] == start_with;
}
bool GctkStrEndWith(const char* str, char end_with) {
	const size_t len = str ? strlen(str) : 0;
	return len > 0 && str[len - 1] == end_with;
}

ssize_t GctkStrFind(const char* str, char c) {
	char* p = strchr(str, c);
	return p == NULL ? -1 : p - str;
}
ssize_t GctkStrFindStr(const char* str, const char* to_find) {
	char* p = strstr(str, to_find);
	return p == NULL ? -1 : p - str;
}
ssize_t GctkStrFindAny(const char* str, const char* chars) {
	while (*chars) {
		ssize_t idx = GctkStrFind(str, *chars++);
		if (idx >= 0) {
			return idx;
		}
	}
	return GCTK_NOT_FOUND;
}
ssize_t GctkStrFindLast(const char* str, char c) {
	char* p = strrchr(str, c);
	return p == NULL ? -1 : p - str;
}
ssize_t GctkStrFindLastStr(const char* str, const char* to_find) {
	const size_t to_find_size = strlen(to_find);
	for (ssize_t i = strlen(str) - to_find_size - 1; i >= 0; i--) {
		if (strncmp(str + i, to_find, to_find_size) == 0) {
			return i;
		}
	}
	return GCTK_NOT_FOUND;
}
ssize_t GctkStrFindAnyLast(const char* str, const char* chars) {
	while (*chars) {
		ssize_t idx = GctkStrFindLast(str, *chars++);
		if (idx >= 0) {
			return idx;
		}
	}
	return GCTK_NOT_FOUND;
}
ssize_t GctkStrFindAnyNot(const char* str, const char* chars) {
	const size_t str_len = strlen(str);
	for (ssize_t i = 0; i < str_len; i++) {
		if (GctkStrFind(chars, str[i]) < 0) {
			return i;
		}
	}
	return GCTK_NOT_FOUND;
}
ssize_t GctkStrFindAnyLastNot(const char* str, const char* chars) {
	for (ssize_t i = strlen(str) - 1; i >= 0; i--) {
		if (GctkStrFind(chars, str[i]) < 0) {
			return i;
		}
	}
	return GCTK_NOT_FOUND;
}

uint32_t GctkHashCRC32(const uint8_t* data, size_t data_size) {
	uint32_t crc = 0xFFFFFFFF;
	for (size_t i = 0; i < data_size; i++) {
		uint8_t byte = data[i];
		crc = crc ^ byte;
		for (int j = 0; j < 8; j++) {
			if (crc & 1) {
				crc = (crc >> 1) ^ GCTK_CRC32_POLY;
			} else {
				crc = crc >> 1;
			}
		}
	}
	return ~crc;
}

hash_t GctkStrHash(const char* cstr) {
	return (hash_t)GCTK_STR_HASH_METHOD((const uint8_t*)cstr, strlen(cstr));
}