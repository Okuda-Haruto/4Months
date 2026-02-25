#pragma once
#include "Object/Object.h"
#include <vector>

#include <Human/Human.h>

class Neck {
public:
	//初期化
	void Initialize(Human* player, const std::shared_ptr<DirectionalLight> directionalLight);
	//更新処理
	void Update();
	//描画処理
	void Draw();
private:

	static std::weak_ptr<DirectionalLight> directionalLight_;

	//モデル
	std::weak_ptr<Model> model_;
	std::vector<std::unique_ptr<Object>> objects_;
	//最終地点
	Vector3 lastPoint_;

	Human* human_ = nullptr;
};