#pragma once

#include "gctk.hpp"
#include "gctk_dll.hpp"

#ifdef _WIN32
#define GCTK_IMPLEMENT_GAME_LAUNCHER \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {\
	int argc;\
	wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);\
	return GctkLauncherMain(argc, argv);\
}
#else
#define GCTK_IMPLEMENT_GAME_LAUNCHER \
int main(const int argc, char** argv) {\
	return GctkLauncherMain(argc, argv);\
}
#endif

#ifdef _WIN32
	#define CLIENT_DLL_NAME "game_client.dll"
	#define SERVER_DLL_NAME "game_server.dll"
	#include <windows.h>
#else
	#define CLIENT_DLL_NAME "libgame_client.so"
	#define SERVER_DLL_NAME "libgame_server.so"
#endif

#ifdef _WIN32
inline int GctkLauncherMain(int argc, wchar_t** argvW) {
	char** argv = new char*[argc];
	args.reserv e(argc);
	for (int i = 0; i < argc; i++) {
		int len = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, nullptr, 0, nullptr, nullptr);
		argv[i] = new char[len + 1];
		WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], len, nullptr, nullptr);
	}
	LocalFree(argvW);
#else
inline int GctkLauncherMain(int argc, char** argv) {
#endif
	int exit_code = 0;

	gctk::Paths::Initialize(argv[0], GCTK_GAME_NAME);

	const gctk::DLL client_dll(gctk::Paths::GameBinaryPath() / CLIENT_DLL_NAME);
	if (!client_dll) {
		LogErrAndPopup("Failed to load library \"{}\"",
			gctk::Paths::GameBinaryPath() / CLIENT_DLL_NAME
		);
#ifdef _WIN32
		for (auto& arg : argv) {
			delete[] arg;
		}
		delete[] argv;
#endif
		return 1;
	}

	const auto client_start = client_dll.get_symbol<void(*)(int argc, char** argv)>("ClientStartup");
	const auto client_update = client_dll.get_symbol<bool(*)()>("ClientUpdate");
	const auto client_render = client_dll.get_symbol<void(*)()>("ClientRender");
	const auto client_shutdown = client_dll.get_symbol<void(*)()>("ClientShutdown");

#ifdef GCTK_SINGLEPLAYER
	const gctk::DLL server_dll(gctk::Paths::GameBinaryPath() / SERVER_DLL_NAME);
	if (!server_dll) {
		LogErrAndPopup("Failed to load library \"{}\"",
			gctk::Paths::GameBinaryPath() / SERVER_DLL_NAME
		);
#ifdef _WIN32
		for (auto& arg : argv) {
			delete[] arg;
		}
		delete[] argv;
#endif
		return 1;
	}
	const auto server_start = server_dll.get_symbol<void(*)(int argc, char** argv)>("ServerStartup");
	const auto server_heartbeat = server_dll.get_symbol<void(*)()>("ServerHeartbeat");
	const auto server_shutdown = server_dll.get_symbol<void(*)()>("ServerShutdown");
#endif

	if (client_start == nullptr) {
		LogErrAndPopup("Could not load symbol \"ClientStartup\" from library \"{}\"", CLIENT_DLL_NAME);
		exit_code = 1;
		goto __GCTK_LAUNCHER_EXIT;
	}

#ifdef GCTK_SINGLEPLAYER
	if (server_start == nullptr) {
		LogErrAndPopup("Could not load symbol \"ServerStartup\" from library \"{}\"", SERVER_DLL_NAME);
		exit_code = 1;
		goto __GCTK_LAUNCHER_EXIT;
	}
	if (server_heartbeat == nullptr) {
		LogErrAndPopup("Could not load symbol \"ServerHeartbeat\" from library \"{}\"", SERVER_DLL_NAME);
		exit_code = 1;
		goto __GCTK_LAUNCHER_EXIT;
	}
#endif

	try {
		client_start(argc, argv);
#ifdef GCTK_SINGLEPLAYER
		server_start(argc, argv);
#endif

		if (client_update != nullptr) {
			bool running = true;
			while (running) {
#ifdef GCTK_SINGLEPLAYER
				server_heartbeat();
#endif
				running = client_update();
				if (client_render != nullptr) {
					client_render();
				}
			}
		}
#ifdef GCTK_SINGLEPLAYER
		if (server_shutdown != nullptr) {
			server_shutdown();
		}
#endif
		if (client_shutdown != nullptr) {
			client_shutdown();
		}
	} catch (const std::exception& ex) {
		LogErrAndPopup("Runtime error: Exception thrown \"{}\"", ex.what());
		exit_code = 1;
	}

__GCTK_LAUNCHER_EXIT:
#ifdef _WIN32
	for (auto& arg : argv) {
		delete[] arg;
	}
	delete[] argv;
#endif
	return exit_code;
}