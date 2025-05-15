#include "GameEngine.h"
#include "Scenes/mainscene.h"

Engine::Application::createInfo CreateAppInfo() {
	Engine::Application::createInfo info;
	info.title = "SandBox";
	info.width = 1000;
	info.height = 500;
	info.defaultScene = new Game::MainScene();
	return info;
}
