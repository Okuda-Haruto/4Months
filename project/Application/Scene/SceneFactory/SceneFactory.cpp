#include "SceneFactory.h"
#include "../BaseScene/GameScene/GameScene.h"
#include "../BaseScene/TitleScene/TitleScene.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName) {
	//次のシーンを生成
	std::unique_ptr<BaseScene> newScene = nullptr;

	if (sceneName == "Game") {
		newScene = std::make_unique<GameScene>();
	} else if (sceneName == "Title") {
		newScene = std::make_unique<TitleScene>();
	}

	return newScene;
}