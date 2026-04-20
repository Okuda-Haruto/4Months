#pragma once
#include <vector>
#include "Math/Collision.h"

class Course;
class Human;
class GameCamera;

class CheckCollision {
public:
	// 初期化
	void Initialize(Course* course, GameCamera* gameCamera);
	// 更新
	void Update(Human* human);
	void UpdateImGui();

	// 予測(当たらないなら止まる座標)
	Vector3 HitPreview(Human* human);
	bool IsPreviewHit() { return isPreviewHit_; };
private:
	void CheckBullet(Human* human);

	// プレイヤーと壁(ボクセル)の判定
	void CheckVoxel(Human* human);

	// プレイヤーの吸い込み判定
	void CheckVacuum(Human* human);

	bool IsHitCapsule(const Vector3& p0, const Vector3& p1, float capsuleRadius, const AABB& aabb);

	Course* course_;
	GameCamera* gameCamera_;
	int mixType_ = 0;
	Vector3 closest_ = {};
	
	// 当たったかどうか
	bool isPreviewHit_ = false;

	//吸い込み速度
	const float baseVacuumSpeed_ = 3.0f;
};

