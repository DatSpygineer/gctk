#include "gctk_client.hpp"
#include "gctk_api.hpp"

#include <vector>
#include <print>

GCTK_GAME_API int ClientMain(const int argc, char** argv) {
	gctk::Client client(argc, argv, GCTK_GAME_NAME);
	client.set_background_color(gctk::Color::CornflowerBlue());
	while (!client.should_exit()) {
		client.update();
	}

	return 0;
}