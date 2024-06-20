#include "gctk/filesys.h"

#define GctkMax(__x__, __max__) ((__x__) > (__max__) ? (__max__) : (__x__))

#ifdef _WIN32
	#include <windows.h>
	#include <direct.h>
	#include <shlobj.h>
#else
	#include <sys/types.h>
	#include <dirent.h>
	#include <unistd.h>
	#include <pwd.h>
#endif

char* GctkPathGetBase(char* buffer, const char* src) {
	char* p = strrchr(src, GCTK_PATH_SEP);
	if (p == NULL) {
		return GctkStrCpy(buffer, src, GCTK_PATH_MAX);
	}

	return GctkStrNCpy(buffer, src, GCTK_PATH_MAX, p - src);
}
char* GctkPathJoin(char* buffer, const char* lhs, const char* rhs) {
	return GctkPathVJoin(buffer, 2, lhs, rhs);
}
char* GctkPathVJoin(char* buffer, size_t count, ...) {
	const char* tokens[count];
	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count; i++) {
		tokens[i] = va_arg(args, const char*);
	}
	va_end(args);
	return GctkPathJoinList(buffer, count, tokens);
}
char* GctkPathJoinList(char* buffer, size_t count, const char** args) {
	for (size_t i = 0; i < count; i++) {
		if (i == 0) {
			GctkStrCpy(buffer, args[i], GCTK_PATH_MAX);
		} else {
			if (!GctkStrEndWith(buffer, GCTK_PATH_SEP) && !GctkStrStartWith(args[i], GCTK_PATH_SEP)) {
				if (!GctkStrAppend(buffer, GCTK_PATH_SEP_STR, GCTK_PATH_MAX)) return NULL;
			}
			if (!GctkStrAppend(buffer, args[i], GCTK_PATH_MAX)) return NULL;
		}
	}

	return buffer;
}
char* GctkPathPop(char* buffer) {
	const ssize_t i = GctkStrFindLast(buffer, GCTK_PATH_SEP);
	if (i < 0) {
		return NULL;
	}
	memset(buffer + i, 0, strlen(buffer) - i - 1);
	return buffer;
}
char* GctkPathAppend(char* buffer, const char* other) {
	if (strlen(buffer) > 0) {
		if (!GctkStrEndWith(buffer, GCTK_PATH_SEP) && !GctkStrStartWith(other, GCTK_PATH_SEP)) {
			GctkStrAppend(buffer, GCTK_PATH_SEP_STR, GCTK_PATH_MAX);
		}
		return GctkStrAppend(buffer, other, GCTK_PATH_MAX);
	}
	return GctkStrCpy(buffer, other, GCTK_PATH_MAX);
}

char* GctkPathGetCurrentDir(char* buffer) {
#ifdef _WIN32
	return _getcwd(buffer, GCTK_PATH_MAX);
#else
	return getcwd(buffer, GCTK_PATH_MAX);
#endif
}

char* GctkPathGetUserBaseDirectory(char* buffer) {
#ifdef _WIN32
	return SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, buffer) == S_OK ? buffer : NULL;
#else
	struct passwd *pw = getpwuid(getuid());
	return GctkStrCpy(buffer, pw->pw_dir, GCTK_PATH_MAX);
#endif
}

bool GctkPathExists(const char* path) {
#ifdef _WIN32
	return GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES;
#else
	struct stat st;
	return stat(path, &st) == 0;
#endif
}
bool GctkPathIsDirectory(const char* path) {
#ifdef _WIN32
	return (GetFileAttributesA(path) & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
	struct stat st;
	return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
#endif
}
bool GctkPathIsFile(const char* path) {
#ifdef _WIN32
	return (GetFileAttributesA(path) & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
	struct stat st;
	return stat(path, &st) == 0 && S_ISREG(st.st_mode);
#endif
}

bool GctkCreateDir(const char* path, bool recursive) {
	if (recursive) {
		char base[GCTK_PATH_MAX];
		if (GctkPathGetBase(base, path) == NULL) return false;
		if (!GctkPathIsDirectory(path)) {
			if (!GctkCreateDir(base, recursive)) {
				return false;
			}
		}
	}
#ifdef _WIN32
	return CreateDirectoryA(path, NULL);
#else
	return mkdir(path, 0666) == 0;
#endif
}
bool GctkDeleteDir(const char* path, bool recursive) {
	if (recursive) {
		DirectoryIterator it;
		if (!GctkStartDirIterator(path, &it)) {
			return false;
		}
		char r_path[GCTK_PATH_MAX] = { 0 };
		bool is_dir;
		do {
			if (GctkDirIteratorCurrent(&it, r_path, &is_dir)) {
				if (!is_dir) {
					GctkDeleteFile(r_path);
				} else {
					GctkDeleteDir(r_path, true);
				}
			}
		} while (GctkDirIteratorNext(&it));
		GctkCloseDirIterator(&it);
	}
#ifdef _WIN32
	return RemoveDirectoryA(path);
#else
	return remove(path) == 0;
#endif
}

bool GctkStartDirIterator(const char* path, DirectoryIterator* iterator) {
	if (iterator == NULL) return false;
#ifdef _WIN32
	char search_pattern[GCTK_PATH_MAX];
	snprintf(search_pattern, GCTK_PATH_MAX, "%s\\*", path);
	iterator->entry = FindFirstFileA(search_pattern, &iterator->data);

	return iterator->entry != INVALID_HANDLE_VALUE;
#else
	iterator->dir = opendir(path);
	if (iterator->dir == NULL) {
		return false;
	}
	iterator->entry = (void*)readdir(iterator->dir);
	GctkStrCpy(iterator->root, path, GCTK_PATH_MAX);
	return true;
#endif
}
bool GctkDirIteratorCurrent(DirectoryIterator* iterator, char* path, bool* is_dir) {
	if (iterator == NULL || path == NULL) return false;
#ifdef _WIN32
	if (GctkStrCpy(path, iterator->data.cFileName, GCTK_PATH_MAX) == 0) return false;
	if (is_dir != NULL) {
		*is_dir = (iterator->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}
#else
	struct dirent* entry = ((struct dirent*)iterator->entry)
	if (entry == NULL) {
		return false;
	}
	snprintf(path, GCTK_PATH_MAX, "%s/%s", iterator->root, entry->d_name);
	struct stat st;
	if (stat(path, &st) < 0) {
		return false;
	}
	if (is_dir != NULL) {
		*is_dir = S_ISDIR(path);
	}
#endif
	return true;
}
bool GctkDirIteratorNext(DirectoryIterator* iterator) {
	if (iterator == NULL) return false;
#ifdef _WIN32
	return FindNextFileA(iterator->entry, &iterator->data);
#else
	return (iterator->entry = readdir(dir)) != NULL;
#endif
}
void GctkCloseDirIterator(DirectoryIterator* iterator) {
	if (iterator == NULL) return;
#ifdef _WIN32
	FindClose(iterator->entry);
#else
	closedir(iterator->dir);
#endif
	memset(iterator, 0, sizeof(DirectoryIterator));
}

bool GctkDeleteFile(const char* path) {
#ifdef _WIN32
	return DeleteFileA(path);
#else
	return remove(path) == 0;
#endif
}
bool GctkCopyFile(const char* from_path, const char* to_path, bool overwrite) {
#ifdef _WIN32
	return CopyFileA(from_path, to_path, !overwrite);
#else
	if (!overwrite && GctkPathExists(to_path)) {
		return false;
	}

	int src = open(from_path, O_RDONLY);
	if (src < 0) {
		return false;
	}
	int dest = open(to_path, O_WRONLY | O_CREAT, 0666);
	if (dest < 0) {
		close(src);
		return false;
	}

	uint8_t temp;
	size_t n;
	while (n = read(src, &temp, 1), n > 0) {
		if (write(dest, &temp, 1) < 1) {
			close(src);
			close(dest);
			return false;
		}
	}

	close(src);
	close(dest);
	return true;
#endif
}
bool GctkMoveFile(const char* from_path, const char* to_path, bool overwrite) {
	if (!overwrite && GctkPathExists(to_path)) {
		return false;
	}
#ifdef _WIN32
	return MoveFileA(from_path, to_path);
#else
	return rename(from_path, to_path) == 0;
#endif
}

FILE* GctkOpenFile(const char* path, GctkFileMode mode, GctkFileCreationFlag flags) {
	char mode_s[4] = { 0 };
	size_t offset = 0;

	switch (mode) {
		case GCTK_FILEMODE_READ: {
			mode_s[0] = 'r';
			offset++;
			if (flags & GCTK_FILE_BINARY) {
				mode_s[1] = 'b';
				offset++;
			}
		} break;
		case GCTK_FILEMODE_WRITE: {
			mode_s[0] = 'w';
			offset++;
			if (flags & GCTK_FILE_BINARY) {
				mode_s[1] = 'b';
				offset++;
			}
		} break;
		case GCTK_FILEMODE_READ_WRITE: {
			mode_s[0] = 'a';
			offset++;
			if (flags & GCTK_FILE_BINARY) {
				mode_s[1] = 'b';
				mode_s[2] = '+';
				offset += 2;
			} else {
				mode_s[1] = '+';
				offset++;
			}
		} break;
		case GCTK_FILEMODE_APPEND: {
			mode_s[0] = 'a';
			offset++;
			if (flags & GCTK_FILE_BINARY) {
				mode_s[1] = 'b';
				offset++;
			}
		} break;
	}

	switch (flags & ~GCTK_FILE_BINARY) {
		case GCTK_FILE_OPEN: {
			/* Nothing to do */
		} break;
		case GCTK_FILE_CREATE_NEW: {
			if (mode == GCTK_FILEMODE_READ || mode == GCTK_FILEMODE_APPEND) {
				mode_s[offset++] = '+';
			}
		} break;
		case GCTK_FILE_OPEN_OR_CREATE: {
			if (mode == GCTK_FILEMODE_WRITE) {
				mode_s[0] = '+';
			}
		} break;
	}

	return fopen(path, mode_s);
}

void GctkFileSeek(FILE* file, ssize_t position) {
	fseek(file, (long)position, SEEK_SET);
}
void GctkFileSeekOffset(FILE* file, ssize_t offset) {
	fseek(file, (long)offset, SEEK_CUR);
}
void GctkFileSeekEnd(FILE* file) {
	fseek(file, 0, SEEK_END);
}

size_t GctkFileSize(FILE* file) {
	long pos = ftell(file);
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, pos, SEEK_SET);
	return size;
}

size_t GctkFileRead(FILE* file, void* dest, size_t read_count, size_t item_size) {
	return fread(dest, item_size, read_count, file);
}
size_t GctkFileReadToEnd(FILE* file, void* dest, size_t max_buffer_size) {
	const size_t size = GctkFileSize(file);
	return GctkFileRead(file, dest, GctkMax(size, max_buffer_size), sizeof(uint8_t));
}
size_t GctkFileWrite(FILE* file, const void* data, size_t write_count, size_t item_size) {
	return fwrite(data, item_size, write_count, file);
}

size_t GctkFileReadLine(FILE* file, char* buffer, size_t max_buffer_size, const char* delims) {
	if (feof(file) || ferror(file)) {
		return 0;
	}

	size_t offset = 0;
	char c;
	while (!feof(file) && offset < max_buffer_size) {
		c = (char)fgetc(file);

		if (GctkStrFind(delims, c) == -1) {
			buffer[offset++] = c;
			continue;
		}
		break;
	}

	buffer[offset] = '\0';
	return offset;
}