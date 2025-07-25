#pragma once

#include <filesystem>

#include "gctk_api.hpp"
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
	#include <dlfcn.h>
#endif

namespace gctk {
	template<typename T>
	concept PointerType = std::is_pointer_v<T>;

	class DLL {
		void* m_pModule;
		bool m_bIsCopy;
	public:
		explicit DLL(const std::filesystem::path& path) : m_bIsCopy(false) {
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
#ifdef _WIN32
			return GetProcAddress((HMODULE)m_pModule, name.c_str());
#else
			return reinterpret_cast<T>(dlsym(m_pModule, name.c_str()));
#endif
		}
	};
}