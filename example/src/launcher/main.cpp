#include "gctk_api.hpp"
#include "gctk_str.hpp"

#ifdef _WIN32
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
int main(const int argc, char** argv) {
	std::vector<std::string> args;
	args.reserve(argc);
	for (int i = 0; i < argc; i++) {
		args.emplace_back(argv[i]);
	}
#endif


	return 0;
}