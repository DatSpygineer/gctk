#pragma once

#include <string>
#include <format>

namespace gctk {
	enum class MessageLevel {
		Info,
		Warning,
		Error
	};

	void Log(const std::string& message, MessageLevel level, const char* file, long line);
	void AssertLog(const std::string& expression, const std::string& failure_message, const char* file, long line);
	void DoCrash(const std::string& message);
	void CloseDebugLog();
}

#ifndef NDEBUG
	#define LogInfo(__message) gctk::Log(__message, gctk::MessageLevel::Info,    __FILE__, __LINE__)
#else
	#define LogInfo(__message)
#endif

#define LogWarn(__message) gctk::Log(__message, gctk::MessageLevel::Warning, __FILE__, __LINE__)
#define LogErr(__message) gctk::Log(__message, gctk::MessageLevel::Error,   __FILE__, __LINE__)
#define FatalError(__message) { LogErr(__message); gctk::DoCrash(__message); }
#define Assert(__expression, __fatal_message) \
	if (!(__expression)) { AssertLog(#__expression, __fatal_message, __FILE__, __LINE__); }
