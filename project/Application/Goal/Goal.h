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
	SRT velocity_;

	//取得しているプレイヤー
	Human* human_;

	float goalCoolTime_ = 0.0f;

	//定位置
	const float kBaseHeight_ = -400;

public:
	// 初期化
	void Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update();

	// 描画
	void Draw();

	//setter
	void SetHuman(Human* human) { human_ = human; goalCoolTime_ = 1.0f; }

	//getter
	SRT GetTransform() { return transform_; }
	Human* GetHuman() { return human_; }
	bool IsCoolTime() { return goalCoolTime_ > 0.0f; }
};