#pragma once
#include "Factory/AbstractSceneFactory/AbstractSceneFactory.h"
#include "../BaseScene/BaseScene.h"

#include <string>

//シーンファクトリー
class SceneFactory : public AbstractSceneFactory {
public:
	//シーン生成
	std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) override;
};