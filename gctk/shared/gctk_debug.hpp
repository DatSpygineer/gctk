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
	#define LogInfo(__message) gctk::Log(__message, gctk::MessageLevel::Info,    __FILE__, __LINE__)
#else
	#define LogInfo(__message)
#endif

#define LogWarn(__message) gctk::Log(__message, gctk::MessageLevel::Warning, __FILE__, __LINE__)
#define LogErr(__message) gctk::Log(__message, gctk::MessageLevel::Error,   __FILE__, __LINE__)
#define FatalError(__message) { LogErr(__message); gctk::DoCrash(__message); }
#define LogErrThrow(__message) { throw gctk::EngineErrorException(__message, __FILE__, __LINE__); }
#define Assert(__expression, __failure_message) \
	if (!(__expression)) { AssertLog(#__expression, __failure_message, false, __FILE__, __LINE__); }
#define AssertFatal(__expression, __failure_message) \
	if (!(__expression)) { AssertLog(#__expression, __failure_message, true, __FILE__, __LINE__); }
#define AssertThrow(__expression, __failure_message) \
	if (!(__expression)) { throw gctk::EngineErrorException(\
			std::format("Assertion of expression \"{}\" has failed: {}", #__expression, __failure_message),\
			__FILE__, __LINE__\
		);\
	}