#include "gctk.hpp"

#include <print>

using namespace gctk;

static Client* client = nullptr;

GCTK_GAME_API void ClientStartup(const int argc, char** argv) {
	client = new Client(argc, argv, GCTK_GAME_NAME, { "assets.gpkg" });
	client->set_background_color(Color::CornflowerBlue());

	Input::CreateAction("test", "enter");

	client->init();
}
GCTK_GAME_API bool ClientUpdate() {
	client->update();
	if (Input::ActionPressed("test")) {
		LogInfo("Test button pressed!");
	} else if (Input::ActionReleased("test")) {
		LogInfo("Test button released!");
	}
	return !client->should_exit();
}
GCTK_GAME_API void ClientRender() {
	client->render();
}
GCTK_GAME_API void ClientShutdown() {
	delete client;
}