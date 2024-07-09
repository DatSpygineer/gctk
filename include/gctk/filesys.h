#ifndef GCTK_FILESYS_H
#define GCTK_FILESYS_H

#include "gctk/common.h"
#include "gctk/str.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
	#define GCTK_PATH_SEP '\\'
	#define GCTK_PATH_SEP_STR "\\"
	#define GCTK_PATH_MAX 260
	#include <windows.h>
#else
	#include <dirent.h>
	#include <limits.h>
	#define GCTK_PATH_SEP '/'
	#define GCTK_PATH_SEP_STR "/"
	#define GCTK_PATH_MAX PATH_MAX
#endif

typedef enum {
	GCTK_FILEMODE_READ,
	GCTK_FILEMODE_WRITE,
	GCTK_FILEMODE_READ_WRITE,
	GCTK_FILEMODE_APPEND
} GctkFileMode;

typedef enum {
	// Open file, error if the file doesn't exist.
	GCTK_FILE_OPEN,
	// Create a new file, overwrites existing file.
	GCTK_FILE_CREATE_NEW,
	// Create a new file, if it doesn't exist, otherwise open.
	GCTK_FILE_OPEN_OR_CREATE,
	// Treat file as a binary file
	GCTK_FILE_BINARY = 4
} GctkFileCreationFlag;

typedef struct {
	void* entry;
#ifdef _WIN32
	WIN32_FIND_DATA data;
#else
	DIR* dir;
	char root[GCTK_PATH_MAX];
#endif
} DirectoryIterator;

GCTK_API char* GctkPathGetBase(char* buffer, const char* src);
GCTK_API char* GctkPathJoin(char* buffer, const char* lhs, const char* rhs);
GCTK_API char* GctkPathVJoin(char* buffer, size_t count, ...);
GCTK_API char* GctkPathJoinList(char* buffer, size_t count, const char** args);
GCTK_API char* GctkPathPop(char* buffer);
GCTK_API char* GctkPathAppend(char* buffer, const char* other);
GCTK_API char* GctkPathGetFileName(char* buffer, size_t max_buffer_size, const char* path);
GCTK_API char* GctkPathGetFileNameNoExt(char* buffer, size_t max_buffer_size, const char* path);
GCTK_API char* GctkPathGetExtension(char* buffer, size_t max_buffer_size, const char* path);
GCTK_API char* GctkPathGetCurrentDir(char* buffer);

GCTK_API char* GctkPathGetUserBaseDirectory(char* buffer);

GCTK_API bool GctkPathExists(const char* path);
GCTK_API bool GctkPathIsDirectory(const char* path);
GCTK_API bool GctkPathIsFile(const char* path);

GCTK_API bool GctkCreateDir(const char* path, bool recursive);
GCTK_API bool GctkDeleteDir(const char* path, bool recursive);

GCTK_API bool GctkStartDirIterator(const char* path, DirectoryIterator* iterator);
GCTK_API bool GctkDirIteratorCurrent(DirectoryIterator* iterator, char* path, bool* is_dir);
GCTK_API bool GctkDirIteratorNext(DirectoryIterator* iterator);
GCTK_API void GctkCloseDirIterator(DirectoryIterator* iterator);

GCTK_API bool GctkDeleteFile(const char* path);
GCTK_API bool GctkCopyFile(const char* from_path, const char* to_path, bool overwrite);
GCTK_API bool GctkMoveFile(const char* from_path, const char* to_path, bool overwrite);

GCTK_API FILE* GctkOpenFile(const char* path, GctkFileMode mode, GctkFileCreationFlag flags);

#define GctkCloseFile fclose
#define GctkFileHasError ferror
#define GctkFlushFile fflush
#define GctkFileAtEof feof

GCTK_API void GctkFileSeek(FILE* file, ssize_t position);
GCTK_API void GctkFileSeekOffset(FILE* file, ssize_t offset);
GCTK_API void GctkFileSeekEnd(FILE* file);
#define GctkFileTell ftell

GCTK_API size_t GctkFileSize(FILE* file);

GCTK_API size_t GctkFileRead(FILE* file, void* dest, size_t read_count, size_t item_size);
GCTK_API size_t GctkFileReadToEnd(FILE* file, void* dest, size_t max_buffer_size);

GCTK_API size_t GctkFileRead_u8 (FILE* file, uint8_t* output);
GCTK_API size_t GctkFileRead_u16(FILE* file, uint16_t* output);
GCTK_API size_t GctkFileRead_u32(FILE* file, uint32_t* output);
GCTK_API size_t GctkFileRead_u64(FILE* file, uint64_t* output);

GCTK_API size_t GctkFileRead_i8 (FILE* file, int8_t* output);
GCTK_API size_t GctkFileRead_i16(FILE* file, int16_t* output);
GCTK_API size_t GctkFileRead_i32(FILE* file, int32_t* output);
GCTK_API size_t GctkFileRead_i64(FILE* file, int64_t* output);

GCTK_API size_t GctkFileRead_f32(FILE* file, float* output);
GCTK_API size_t GctkFileRead_f64(FILE* file, double* output);

GCTK_API size_t GctkFileRead_str(FILE* file, char* buffer, size_t buffer_max_size);

GCTK_API size_t GctkFileReadLine(FILE* file, char* buffer, size_t max_buffer_size, const char* delims);

GCTK_API size_t GctkFileWrite(FILE* file, const void* data, size_t write_count, size_t item_size);

GCTK_API size_t GctkFileWrite_u8(FILE* file,  uint8_t data);
GCTK_API size_t GctkFileWrite_u16(FILE* file, uint16_t data);
GCTK_API size_t GctkFileWrite_u32(FILE* file, uint32_t data);
GCTK_API size_t GctkFileWrite_u64(FILE* file, uint64_t data);

GCTK_API size_t GctkFileWrite_i8(FILE* file,  int8_t data);
GCTK_API size_t GctkFileWrite_i16(FILE* file, int16_t data);
GCTK_API size_t GctkFileWrite_i32(FILE* file, int32_t data);
GCTK_API size_t GctkFileWrite_i64(FILE* file, int64_t data);

GCTK_API size_t GctkFileWrite_f32(FILE* file, float  data);
GCTK_API size_t GctkFileWrite_f64(FILE* file, double data);

GCTK_API size_t GctkFileWrite_str(FILE* file, const char* data);

GCTK_API size_t GctkFileWriteLine(FILE* file, const char* data);

#endif // GCTK_FILESYS_H