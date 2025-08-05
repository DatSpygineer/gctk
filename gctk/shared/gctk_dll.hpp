#pragma once

#include <filesystem>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
	#include <dlfcn.h>
#endif

#include "gctk_filesys.hpp"

namespace gctk {
	template<typename T>
	concept PointerType = std::is_pointer_v<T>;

	class DLL {
		void* m_pModule;
		bool m_bIsCopy;
		Path m_sPath;
	public:
		explicit DLL(const Path& path) : m_bIsCopy(false), m_sPath(path) {
#ifdef _WIN32
			m_pModule = (void*)(LoadLibraryW(path.c_str()));
#else
			m_pModule = dlopen(path.c_str(), RTLD_NOW);
#endif
		}
		DLL(const DLL& dll) : m_pModule(dll.m_pModule), m_bIsCopy(true) {}
		DLL(DLL&& dll) noexcept : m_pModule(dll.m_pModule), m_bIsCopy(false) { dll.m_pModule = nullptr; }
		~DLL() {
			if (m_pModule != nullptr) {
#ifdef _WIN32
				FreeLibrary(((HMODULE))(m_pModule));
#else
				dlclose(m_pModule);
#endif
				m_pModule = nullptr;
			}
		}

		template<PointerType T>
		inline T get_symbol(const std::string& name) const {
			const char* name_cstr = name.c_str();
			T result = nullptr;
#ifdef _WIN32
			result = reinterpret_cast<T>(GetProcAddress((HMODULE)m_pModule, name_cstr));
#else
			result = reinterpret_cast<T>(dlsym(m_pModule, name_cstr));
#endif
			if (result == nullptr) {
				LogErr(std::format("Failed to load symbol \"{}\" from DLL \"{}\"", name_cstr, m_sPath));
			}
			return result;
		}

		[[nodiscard]] constexpr bool is_valid() const noexcept { return m_pModule != nullptr; }
		[[nodiscard]] constexpr operator bool() const noexcept { return m_pModule != nullptr; }
	};
}