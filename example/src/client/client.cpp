#include "gctk_client.hpp"
#include "gctk_api.hpp"

#include <vector>
#include <print>

GCTK_GAME_API int ClientMain(const std::vector<std::string>& args) {
	std::println("Hello world!");
	return 0;
}