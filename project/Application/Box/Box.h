#pragma once
#include <GameEngine.h>
#include <Shape/Sphere.h>

class Course;

class Box {
private:
	std::unique_ptr<Object> object_;

	//トランスフォーム
	SRT transform_;
	//速度
	Vector3 velocity_;

	//衝突判定
	Sphere collider_;

	int32_t MaxHP_;
	int32_t HP_;

	bool isDead_;

	Course* course_;

public:

	void Initialize(Course* course, SRT transform, Vector3 velocity, const float radius, const int32_t maxHP, std::shared_ptr<DirectionalLight> directionalLight);

	void Update();

	void Draw();

	// 移動
	void Move(const Vector3& velocity);

	void Damage(){ HP_--;}

	Object* GetObjectData() { return object_.get(); }

	//getter
	SRT GetTransform() { return transform_; }

	//setter
	void SetTransform(SRT transform) { transform_ = transform; }
	Sphere GetCollider() { return collider_; }

	bool IsDead() { return isDead_; }
};