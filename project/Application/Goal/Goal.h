#pragma once
#include "Human/Human.h"
#include <Object/Object.h>

#include <memory>

class Goal {
private:
	//モデル
	std::unique_ptr<Object> object_;

	//トランスフォーム
	SRT transform_;

	//取得しているプレイヤー
	Human* human_;
public:
	// 初期化
	void Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update();

	// 描画
	void Draw();

	//setter
	void SetHuman(Human* human) { human_ = human; }

	//getter
	SRT GetTransform() { return transform_; }
	Human* GetHuman() { return human_; }
};