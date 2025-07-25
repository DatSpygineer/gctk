#include "gctk_api.hpp"
#include "gctk_debug.hpp"
#include "gctk_str.hpp"
#include "gctk_dll.hpp"
#include "gctk_paths.hpp"

#ifdef _WIN32
#define CLIENT_DLL_NAME "game_client.dll"

#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	int argc;
	LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
	std::vector<std::string> args;
	args.reserve(argc);
	for (int i = 0; i < argc; i++) {
		int len = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, nullptr, 0, nullptr, nullptr);
		std::string arg;
		arg.reserve(len);
		WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, arg.data(), len, nullptr, nullptr);
		arg.pop_back()
		args.push_back(arg);
	}
	LocalFree(argvW);
#else
#define CLIENT_DLL_NAME "libgame_client.so"

int main(const int argc, char** argv) {
	std::vector<std::string> args;
	args.reserve(argc);
	for (int i = 0; i < argc; i++) {
		args.emplace_back(argv[i]);
	}
#endif
	gctk::DLL client_dll(gctk::Paths::bin_path() / CLIENT_DLL_NAME);
	auto client_main = client_dll.get_symbol<int(*)(const std::vector<std::string>&)>("ClientMain");
	if (client_main != nullptr) {
		return client_main(args);
	}
	FatalError(std::format("Could not load symbol \"ClientMain\" from library \"{}\"", CLIENT_DLL_NAME));
	return 1;
}