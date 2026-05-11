#define NOMINMAX
#include "CheckCollision.h"
#include "Human/Human.h"
#include "Course/Course.h"
#include "GameCamera/GameCamera.h"
#include "Mix.h"

void CheckCollision::Initialize(Course* course, GameCamera* gameCamera) {
	course_ = course;
	gameCamera_ = gameCamera;
}

void CheckCollision::Update(Human* human) {
	CheckBullet(human);
	CheckVacuum(human);
	CheckVoxel(human);
}

void CheckCollision::UpdateImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("かき混ぜ");
	ImGui::SliderInt("挙動", &mixType_, 0, 1);
	ImGui::End();
#endif
}

void CheckCollision::CheckBullet(Human* human) {
	if (!human->IsShooting()) return;

	Sphere prev = { human->GetPrevVacuumSphere().center, 3.0f };
	Sphere curr = { human->GetVacuumSphere().center, 3.0f };

	Vector3 p0 = prev.center;
	Vector3 p1 = curr.center;
	float radius = prev.radius;

	// ボクセル
	course_->GetVoxel()->Collision(Sphere{ Lerp(p0, p1,0.25f) ,radius });
	course_->GetVoxel()->Collision(Sphere{ Lerp(p0, p1,0.50f) ,radius });
	course_->GetVoxel()->Collision(Sphere{ Lerp(p0, p1,0.75f) ,radius });
	course_->GetVoxel()->Collision(curr);

	for (auto& box : course_->GetBoxes()) {
		AABB boxAABB = box->GetCollider();

		boxAABB.min -= {0.1f, 0.1f, 0.1f};
		boxAABB.max += {0.1f, 0.1f, 0.1f};

		if (IsHitCapsule(p0, p1, radius, boxAABB)) {
			human->StopBullet(closest_);
			return;
		}
	}
}

void CheckCollision::CheckVoxel(Human* human) {
	if (human->IsInvincible()) { return; }
	Sphere prev = { human->GetTransform().translate, 3.0f };
	Sphere curr = { prev.center + human->GetVelocity() * GameEngine::GetDeltaTimeRate(), 3.0f};

	Vector3 p0 = prev.center;
	Vector3 p1 = curr.center;
	float radius = prev.radius;

	//ボクセル
	course_->GetVoxel()->Collision(Sphere{ Lerp(p0, p1,0.25f) ,radius});
	course_->GetVoxel()->Collision(Sphere{ Lerp(p0, p1,0.50f) ,radius });
	course_->GetVoxel()->Collision(Sphere{ Lerp(p0, p1,0.75f) ,radius });
	course_->GetVoxel()->Collision(curr);

	for (auto& box : course_->GetBoxes()) {
		AABB boxAABB = box->GetCollider();

		boxAABB.min -= {0.1f, 0.1f, 0.1f};
		boxAABB.max += {0.1f, 0.1f, 0.1f};

		// 判定
		if (IsHitCapsuleHuman(p0, p1, radius, boxAABB)) {
			// 衝突
			if (box->GetTransform().scale.x > 1.0f) {
				if (human->CanShoot()) {
					// 跳ね上がるため再ヒット防止
					human->OnHitVoxel(boxAABB);
					box->Damage(4);
					break;
				}

				human->OnHitVoxel(boxAABB);
				box->Damage(4);

			} else {
				box->Break();
			}
		}
	}
}

void CheckCollision::CheckVacuum(Human* human) {
	if (human->IsVacuuming()) {
		Sphere vacuumSphere = human->GetVacuumSphere();

		//ボクセル
		course_->GetVoxel()->Collision(vacuumSphere);

		//ボックス(吸い込まれてるボクセル)
		for (auto& box : course_->GetBoxes()) {
			AABB boxAABB = box->GetCollider();
			// 判定
			if (IsCollision(boxAABB, vacuumSphere)) {
				Vector3 boxPos = box->GetTransform().translate;
				// 衝突
				Vector3 axis = Vector3(0, 1, 0); // Y軸回転

				float length = Length(vacuumSphere.center - boxPos);
				//割合で速度を変える
				float vacuumLate = length / vacuumSphere.radius;
				//速度
				float speed = (1.0f - vacuumLate) * baseVacuumSpeed_ * GameEngine::GetDeltaTimeRate();

				if (mixType_ == 0)box->Move(Mix(boxPos, vacuumSphere.center, axis, 1.5f * GameEngine::GetDeltaTimeRate(), vacuumSphere.radius, false) * speed);
				if (mixType_ == 1)box->Move(Mix(boxPos, vacuumSphere.center, axis, 1.5f * GameEngine::GetDeltaTimeRate(), vacuumSphere.radius, true) * speed);

				if (length < vacuumSphere.radius * 0.66f) {
					box->Damage(GameEngine::GetDeltaTimeRate());
				}
			}
		}
	}
}

// AABB上の最近点
Vector3 ClosestPointAABB(const Vector3& p, const AABB& aabb) {
	Vector3 result;
	result.x = std::clamp(p.x, aabb.min.x, aabb.max.x);
	result.y = std::clamp(p.y, aabb.min.y, aabb.max.y);
	result.z = std::clamp(p.z, aabb.min.z, aabb.max.z);
	return result;
}

bool CheckCollision::IsHitCapsule(
	const Vector3& p0,
	const Vector3& p1,
	float capsuleRadius,
	const AABB& aabb) {
	// AABBの中心
	Vector3 center = (aabb.min + aabb.max) * 0.5f;

	Vector3 seg = p1 - p0;
	float segLenSq = Dot(seg, seg);

	float t = 0.0f;
	if (segLenSq > 0.0f) {
		t = Dot(center - p0, seg) / segLenSq;
		t = std::clamp(t, 0.0f, 1.0f);
	}

	closest_ = p0 + seg * t;

	// AABB上の最近点
	Vector3 closestOnBox = ClosestPointAABB(closest_, aabb);

	// 距離チェック
	Vector3 diff = closest_ - closestOnBox;
	float distSq = Dot(diff, diff);

	return distSq <= (capsuleRadius * capsuleRadius);
}

bool CheckCollision::IsHitCapsuleHuman(
	const Vector3& p0,
	const Vector3& p1,
	float capsuleRadius,
	const AABB& aabb)
{
	Segment C_segment;
	C_segment.origin = p0;
	C_segment.diff = p1 - p0;
	AABB C_aabb = aabb;
	C_aabb.min -= Vector3{ capsuleRadius, capsuleRadius, capsuleRadius};
	C_aabb.max += Vector3{ capsuleRadius, capsuleRadius, capsuleRadius };


	return IsCollision(C_aabb, C_segment);
}

Vector3 CheckCollision::HitPreview(Human* human) {
	Sphere start = { human->GetTransform().translate, 3.0f };
	Sphere goal = { human->GetVacuumStartPos(), 3.0f };

	Vector3 p0 = start.center;
	Vector3 p1 = goal.center;
	float radius = start.radius;

	// ボクセル
	Vector3 dir = Normalize(goal.center - start.center);
	Vector3 curr = start.center;
	while (curr.y > goal.center.y) {
		// 当たっていればその座標
		std::optional<Vector3> hitPosOpt = course_->GetVoxel()->CollisionCheck({ curr,3.0f });
		if (hitPosOpt.has_value()) {
			isPreviewHit_ = true;
			return hitPosOpt.value();
		}
		curr += dir * (goal.radius / 2.0f);
	}
	course_->GetVoxel()->Collision(goal);

	for (auto& box : course_->GetBoxes()) {
		AABB boxAABB = box->GetCollider();

		if (IsHitCapsule(p0, p1, radius, boxAABB)) {
			isPreviewHit_ = true;
			return closest_ - dir * 3.2f;
		}
	}

	isPreviewHit_ = false;
	return goal.center;
}