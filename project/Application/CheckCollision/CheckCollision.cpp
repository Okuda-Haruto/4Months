#include "CheckCollision.h"
#include "Human/Human.h"
#include "Course/Course.h"
#include "Goal/Goal.h"
#include "Neck/Neck.h"
#include "Math/Collision.h"

void CheckCollision::Initialize(Course* course, Goal* goal, std::vector<Neck*> necks) {
	course_ = course;
	goal_ = goal;
	necks_ = necks;
}

void CheckCollision::Update(Human* human) {
	CheckRing(human);
	CheckSpike(human);
	CheckWall(human);
	CheckNeck(human);
	CheckGoal(human);
}

void CheckCollision::CheckRing(Human* human) {
	for (auto& ring : course_->GetRings()) {
		Vector3 ringCenter = ring->GetColliderCenter();
		float ringHeight = ring->GetColliderHeight();
		Vector3 playerPos = human->GetTransform().translate;

		// 高さの判定
		if (fabsf(ringCenter.y - playerPos.y) <= ringHeight / 2.0f) {
			// 横の判定
			float ringRadius = ring->GetColliderRadius();
			if (Length(Vector2{ ringCenter.x, ringCenter.z } - Vector2{ playerPos.x, playerPos.z }) <= ringRadius) {
				// 衝突
				if (!ring->IsCoolDown(human->GetID())) { // 連続で触れられない
					human->OnHitRing(ring->GetBoostAmount(), ring->GetBoostMaxAmount());
					ring->OnCollide(human->GetID());
				}
			}
		}
	}
}

void CheckCollision::CheckSpike(Human* human) {
	for (auto& spike : course_->GetSpikes()) {
		Sphere spikeSphere = spike->GetCollider();
		Vector3 playerPos = human->GetTransform().translate;
		Sphere playerSphere = { playerPos, 1.0f };

		// 判定
		if (IsCollision(spikeSphere, playerSphere)) {
			// 衝突
			spike->OnCollide();
			human->OnHitSpike(spikeSphere.center);
			goal_->SetHuman(nullptr);

		}
	}
}

void CheckCollision::CheckWall(Human* human) {
	for (auto& wall : course_->GetWalls()) {
		Vector3 playerPos = human->GetTransform().translate;
		Sphere playerSphere = { playerPos, 1.0f };

		// 判定
		if (fabsf(wall.center.y - playerPos.y) >= wall.size.y) { // 高さが合っていたら詳細な判定
			if (IsCollision(wall, playerSphere)) {
				// 衝突
				course_->OnCollide();
				human->OnHitWall(wall);
			}
		}
	}
}

void CheckCollision::CheckNeck(Human* human) {
	if (human->isDrifting_ && human->IsCoilAround()) { return; }
	for (auto& neck : necks_) {
		Vector3 playerPos = human->GetTransform().translate;
		Sphere playerSphere = { playerPos, 1.0f };
		const auto& transforms = neck->GetTransforms();
		// 最新の2つは判定無視
		if (transforms.size() > 2) {
			for (size_t i = 0; i < transforms.size() - 2; ++i) {
				const auto& nTransform = transforms[i];
				Vector3 nPos = nTransform.translate;

				// 判定
				if (fabsf(nPos.y - playerPos.y) >= nTransform.scale.y) { // 高さが合っていたら詳細な判定
					Sphere nSphere = { nPos, 0.5f };
					if (IsCollision(nSphere, playerSphere)) {
						// 衝突
						human->OnHitNeck(nPos);
						//goal_->SetHuman(nullptr);
					}
				}
			}
		}
	}
}

void CheckCollision::CheckGoal(Human* human) {

	if (goal_->IsCoolTime()) return;

	Vector3 playerPos = human->GetTransform().translate;
	Sphere playerSphere = { playerPos, 1.0f };
	Vector3 goalPos = goal_->GetTransform().translate;
	Sphere goalSphere = { goalPos, 2.0f };

	// 判定
	if (IsCollision(goalSphere, playerSphere)) {
		// 衝突
		goal_->SetHuman(human);
	}
}