#include "gctk_debug.hpp"

#include <chrono>
#include <print>

#ifdef _WIN32
#include <windows.h>
#endif

static constexpr std::string_view s_loglevel_names[] = {
	"INFO",
	"WARN"
	"ERROR"
};

namespace gctk {
	void Log(const std::string& message, MessageLevel level, const char* file, long line) {
		auto now = std::chrono::system_clock::now();
		const auto formatted_msg = std::format(
			"{} [{}] \"{}\":{} {}",
			now,
			s_loglevel_names[static_cast<int>(level)],
			file, line,
			message
		);

#ifdef _WIN32
#else
		switch (level) {
			case MessageLevel::Warning: std::print("\x1B[33;1m"); break;
			case MessageLevel::Error: std::print("\x1B[31;1m"); break;
			default: /* Nothing to do */ break;
		}
#endif
		std::println("{}", formatted_msg);
#ifdef _WIN32
#else
		if (level != MessageLevel::Info) {
			std::print("\x1B[0m");
		}
#endif
	}

	void AssertLog(const std::string& expression, const std::string& failure_message, const char* file, long line) {
		Log(std::format(R"(Assertion of expression "{}" failed: "{}")", expression, failure_message), MessageLevel::Error, file, line);
	}

	void DoCrash(const std::string& message) {
#ifdef _WIN32
		MessageBoxA(nullptr, message.c_str(), "Fatal Error!", MB_OK | MB_ICONERROR);
#else
		system(std::format(
			R"({} --error "Game has crashed!\n{}" --title "Fatal Error")",
			std::system("command -v kdialog > /dev/null 2>&1") == 0 ? "kdialog" : "zenity",
			message
		).c_str());
#endif
		exit(1);
	}
}
