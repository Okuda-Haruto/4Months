#pragma once
#include <Object/Object.h>

#include <memory>

class Goal {
private:
	//モデル
	std::unique_ptr<Object> object_;

	//トランスフォーム
	SRT transform_;
public:
	// 初期化
	void Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight);

	// 更新
	void Update();

	// 描画
	void Draw();

	SRT GetTransform() { return transform_; }
};