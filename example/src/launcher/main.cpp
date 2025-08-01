#include "gctk_debug.hpp"
#include "gctk_str.hpp"
#include "gctk_dll.hpp"
#include "gctk_filesys.hpp"

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
	gctk::Paths::init(argv[0], GCTK_GAME_NAME);

	const gctk::DLL client_dll(gctk::Paths::bin_path() / CLIENT_DLL_NAME);
	const auto client_start = client_dll.get_symbol<void(*)(int argc, char** argv)>("ClientStartup");
	const auto client_update = client_dll.get_symbol<bool(*)()>("ClientUpdate");
	const auto client_render = client_dll.get_symbol<void(*)()>("ClientRender");
	const auto client_shutdown = client_dll.get_symbol<void(*)()>("ClientShutdown");

	if (client_start != nullptr) {
		client_start(argc, argv);

		if (client_update != nullptr) {
			while (client_update()) {
				if (client_render != nullptr) {
					client_render();
				}
			}
		}

		if (client_shutdown != nullptr) {
			client_shutdown();
		}
#ifdef _WIN32
		for (auto& arg : argv) {
			delete[] arg;
		}
		delete[] argv;
#endif
		return 0;
	}
	FatalError(std::format("Could not load symbol \"ClientMain\" from library \"{}\"", CLIENT_DLL_NAME));
	return 1;
}