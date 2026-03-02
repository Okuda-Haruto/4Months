#include "SceneFactory.h"
#include "../BaseScene/GameScene/GameScene.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName) {
	//次のシーンを生成
	std::unique_ptr<BaseScene> newScene = nullptr;

	if (sceneName == "Game") {
		newScene = std::make_unique<GameScene>();
	}

	return newScene;
}