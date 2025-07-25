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
	char** argv = new char*[argc];
	args.reserv e(argc);
	for (int i = 0; i < argc; i++) {
		int len = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, nullptr, 0, nullptr, nullptr);
		argv[i] = new char[len + 1];
		WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], len, nullptr, nullptr);
	}
	LocalFree(argvW);
#else
#define CLIENT_DLL_NAME "libgame_client.so"

int main(const int argc, char** argv) {
#endif
	const gctk::DLL client_dll(gctk::Paths::bin_path() / CLIENT_DLL_NAME);
	const auto client_main = client_dll.get_symbol<int(*)(int, char**)>("ClientMain");
	if (client_main != nullptr) {
		const int exit_code = client_main(argc, argv);
#ifdef _WIN32
		for (auto& arg : argv) {
			delete[] arg;
		}
		delete[] argv;
#endif
		return exit_code;
	}
	FatalError(std::format("Could not load symbol \"ClientMain\" from library \"{}\"", CLIENT_DLL_NAME));
	return 1;
}