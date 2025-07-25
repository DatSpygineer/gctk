#pragma once

#include "gctk_api.hpp"

namespace gctk {
	enum class MessageLevel {
		Info,
		Warning,
		Error
	};

	GCTK_API void Log(const std::string& message, MessageLevel level, const char* file, long line);
	GCTK_API void AssertLog(const std::string& expression, const std::string& failure_message, const char* file, long line);
	GCTK_API void DoCrash(const std::string& message);
}

#define LogInfo(__message) gctk::Log(__message, gctk::MessageLevel::Info,    __FILE__, __LINE__)
#define LogWarn(__message) gctk::Log(__message, gctk::MessageLevel::Warning, __FILE__, __LINE__)
#define LogErr(__message) gctk::Log(__message, gctk::MessageLevel::Error,   __FILE__, __LINE__)
#define FatalError(__message) { LogErr(__message); gctk::DoCrash(__message); }
#define Assert(__expression, __fatal_message) \
	if (!(__expression)) { AssertLog(#__expression, __fatal_message, __FILE__, __LINE__); }
