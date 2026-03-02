#pragma once
#include <string>
#include <memory>

class BaseScene;

//概念シーンファクトリー
class AbstractSceneFactory {
public:
	//仮想デストラクタ
	virtual ~AbstractSceneFactory() = default;
	virtual std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) = 0;
};