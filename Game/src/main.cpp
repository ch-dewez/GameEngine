#include "Application.h"
#include "Scenes/mainscene.h"
#include "filesystem"
#include "iostream"

int main() {

    std::cout << std::filesystem::current_path() << std::endl;
  Game::MainScene* mainscene = new Game::MainScene();
  Application app("Game", 1000, 500, mainscene);
  app.Run();
}
