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

	int8_t number_;
	//どれだけ吸引されやすいか
	float vacuumSensitivity_;

	//衝突判定
	AABB collider_;

	int32_t MaxHP_;
	int32_t HP_;

	bool isDead_;

	Course* course_;

public:

	void Initialize(Course* course, SRT transform, Vector3 velocity, int8_t number, float vacuumSensitivity, const int32_t maxHP, std::shared_ptr<DirectionalLight> directionalLight);

	void Update();

	void Draw();

	// 移動
	void Move(const Vector3& velocity);

	void Damage(int damage = 1){ HP_ -= damage;}
	void Break();

	Object* GetObjectData() { return object_.get(); }

	//getter
	SRT GetTransform() { return transform_; }
	AABB GetCollider() { return collider_; }
	int8_t GetNumber() { return number_; }
	bool IsDead() { return isDead_; }

	//setter
	void SetTransform(SRT transform) { transform_ = transform; }
};