#pragma once
#include <GameEngine.h>
#include <Shape/Sphere.h>

class Box {
private:
	std::unique_ptr<Object> object_;

	//トランスフォーム
	SRT transform_;

	//衝突判定
	Sphere collider_;

public:

	void Initialize(SRT transform, const float radius, std::shared_ptr<DirectionalLight> directionalLight);

	void Update();

	void Draw();

	// 移動
	void Move(const Vector3& velocity);

	//getter
	SRT GetTransform() { return transform_; }

	//setter
	void SetTransform(SRT transform) { transform_ = transform; }
	Sphere GetCollider() { return collider_; }
};