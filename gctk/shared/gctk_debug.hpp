#pragma once

#include <chrono>
#include <string>
#include <format>

namespace gctk {
	enum class MessageLevel {
		Info,
		Warning,
		Error
	};

	void Log(const std::string& message, MessageLevel level, const char* file, long line);
	void AssertLog(const std::string& expression, const std::string& failure_message, bool fatal, const char* file, long line);
	void DoCrash(const std::string& message);
	void CloseDebugLog();
	void ErrorPopup(const std::string& message);

	class EngineErrorException final : public std::exception {
		std::string m_sMessage;
		std::string m_sCallerFileName;
		long m_iCallerLine;
		std::chrono::time_point<std::chrono::system_clock> m_tTimeStamp;
	public:
		EngineErrorException(const std::string& message, const char* caller_file, const long caller_line) :
			m_sMessage(message), m_sCallerFileName(caller_file), m_iCallerLine(caller_line),
			m_tTimeStamp(std::chrono::system_clock::now()) { }
		EngineErrorException(std::string&& message, const char* caller_file, const long caller_line) noexcept :
		m_sMessage(std::move(message)), m_sCallerFileName(caller_file), m_iCallerLine(caller_line),
		m_tTimeStamp(std::chrono::system_clock::now()) { }

		[[nodiscard]] constexpr const char* what() const noexcept override { return m_sMessage.c_str(); }
		[[nodiscard]] constexpr const char* caller_filename() const noexcept { return m_sCallerFileName.c_str(); }
		[[nodiscard]] constexpr long caller_line() const noexcept { return m_iCallerLine; }

		[[nodiscard]] inline std::string message() const {
			return std::format(
				"{:%Y.%m.%d %H:%M:%S} [EXCEPTION] \"{}\":{} {}",
				std::chrono::system_clock::now(),
				m_sCallerFileName, m_iCallerLine,
				m_sMessage
			);
		}
	};
}

#ifndef NDEBUG
	#define LogInfo(__message, ...) gctk::Log(std::format(__message, ##__VA_ARGS__), gctk::MessageLevel::Info,    __FILE__, __LINE__)
#else
	#define LogInfo(__message)
#endif

#define LogWarn(__message, ...) gctk::Log(std::format(__message, ##__VA_ARGS__), gctk::MessageLevel::Warning, __FILE__, __LINE__)
#define LogErr(__message, ...) gctk::Log(std::format(__message, ##__VA_ARGS__), gctk::MessageLevel::Error,   __FILE__, __LINE__)
#define LogErrAndPopup(__message, ...) gctk::Log(std::format(__message, ##__VA_ARGS__), gctk::MessageLevel::Error,   __FILE__, __LINE__); gctk::ErrorPopup(__message)
#define FatalError(__message, ...) { LogErr(__message, ##__VA_ARGS__); gctk::DoCrash(std::format(__message, ##__VA_ARGS__)); }
#define LogErrThrow(__message, ...) { LogErr(__message, ##__VA_ARGS__); throw gctk::EngineErrorException(std::format(__message, ##__VA_ARGS__), __FILE__, __LINE__); }
#define Assert(__expression, __failure_message, ...) \
	if (!(__expression)) { AssertLog(#__expression, std::format(__failure_message, ##__VA_ARGS__), false, __FILE__, __LINE__); }
#define AssertFatal(__expression, __failure_message, ...) \
	if (!(__expression)) { AssertLog(#__expression, std::format(__failure_message, ##__VA_ARGS__), true, __FILE__, __LINE__); }
#define AssertThrow(__expression, __failure_message, ...) \
	if (!(__expression)) { throw gctk::EngineErrorException(\
			std::format("Assertion of expression \"{}\" has failed: {}", #__expression, std::format(__failure_message, ##__VA_ARGS__)),\
			__FILE__, __LINE__\
		);\
	}