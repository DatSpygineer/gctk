#include "gctk_debug.hpp"

#include <fstream>
#include <chrono>
#include <print>

#include "gctk_version.hpp"
#include "gctk_filesys.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

static constexpr std::string_view s_loglevel_names[] = {
	"INFO",
	"WARN",
	"ERROR"
};

namespace gctk {
	static std::ofstream s_logfile;
	static bool s_no_filelog = false;

	static std::ofstream& GetLogFile();

	void Log(const std::string& message, MessageLevel level, const char* file, long line) {
		const auto formatted_msg = std::format(
			"{} - {:%Y.%m.%d %H:%M:%S} [{}] \"{}\":{} {}",
#ifdef GCTK_CLIENT
			"CLIENT",
#else
			"SERVER",
#endif
			std::chrono::system_clock::now(),
			s_loglevel_names[static_cast<int>(level)],
			file, line,
			message
		);

		if (!s_no_filelog) {
			if (auto& log_file = GetLogFile(); log_file.is_open()) {
				log_file << formatted_msg << std::endl;
			}
		}
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

	void AssertLog(const std::string& expression, const std::string& failure_message, bool fatal, const char* file, long line) {
		Log(std::format(R"(Assertion of expression "{}" failed: "{}")", expression, failure_message),
			MessageLevel::Error,
			file, line
		);
		if (fatal) {
			DoCrash(failure_message);
		}
	}

	void DoCrash(const std::string& message) {
		ErrorPopup(message);
		std::println("{:%Y.%m.%d %H:%M:%S} - Game has crashed! \"{}\"", std::chrono::system_clock::now(), message);
		if (s_logfile.is_open()) {
			s_logfile << std::format("{:%Y.%m.%d %H:%M:%S} - Game has crashed! \"{}\"", std::chrono::system_clock::now(), message) << std::endl;
			s_logfile.flush();
			s_logfile.close();
		}
		exit(1);
	}

	void ErrorPopup(const std::string& message) {
#ifdef _WIN32
		MessageBoxA(nullptr, message.c_str(), "Fatal Error!", MB_OK | MB_ICONERROR);
#else
		system(std::format(
			R"({} --error "{}" --title "Fatal Error")",
			std::system("command -v kdialog > /dev/null 2>&1") == 0 ? "kdialog" : "zenity",
			message
		).c_str());
#endif
	}

	void CloseDebugLog() {
		if (s_logfile.is_open()) {
			s_logfile.flush();
			s_logfile.close();
		}
	}

	static std::ofstream& GetLogFile() {
		if (!s_logfile.is_open()) {
			auto path = Paths::GameBasePath() / "logs";
			if (!Paths::exists(path)) {
				Paths::create_directories(path);
			}
			path /= std::format("log_{:%Y_%m_%d}.txt", std::chrono::system_clock::now());
			if (Paths::exists(path)) {
				s_logfile = std::ofstream(path, std::ios::out | std::ios::app);
			} else {
				s_logfile = std::ofstream(path);
			}

			s_no_filelog = true; // Set to avoid infinite recursive calls
			if (!s_logfile.is_open()) {
				LogWarn("Failed to open log file \"{}\"", path);
			} else {
				LogInfo("Opened log file \"{}\"", path);
				s_logfile << "================ GCTk v" << EngineVersion << " ================" << std::endl;
			}
			s_no_filelog = false;
		}
		return s_logfile;
	}
}
