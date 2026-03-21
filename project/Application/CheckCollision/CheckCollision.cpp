#include "CheckCollision.h"
#include "Human/Human.h"
#include "Course/Course.h"
#include "Goal/Goal.h"
#include "Neck/Neck.h"
#include "Math/Collision.h"
#include "GameCamera/GameCamera.h"
#include "Mix.h"

void CheckCollision::Initialize(Course* course, Goal* goal, std::vector<Neck*> necks, GameCamera* gameCamera) {
	course_ = course;
	goal_ = goal;
	necks_ = necks;
	gameCamera_ = gameCamera;
}

void CheckCollision::Update(Human* human) {
	CheckRing(human);
	CheckSpike(human);
	CheckEnergy(human);
	//CheckWall(human);
	CheckNeck(human);
	CheckGoal(human);
	CheckVacuum(human);

#ifdef USE_IMGUI
	if (human->GetID() == 0) {
		ImGui::Begin("かき混ぜ");
		ImGui::Text("タイプ1:吸い込み、タイプ2:放出");
		ImGui::SliderInt("挙動", &mixType_, 0, 1);
		ImGui::End();
	}
#endif
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
	if (human->IsInvincible()) { return; }
	for (auto& spike : course_->GetSpikes()) {
		Sphere spikeSphere = spike->GetCollider();
		Vector3 playerPos = human->GetTransform().translate;
		Sphere playerSphere = { playerPos, 0.5f };

		// 判定
		if (IsCollision(spikeSphere, playerSphere)) {
			// 衝突
			spike->OnCollide();
			human->OnHitSpike(spikeSphere.center);
			if (goal_->GetHuman() == human) {
				goal_->SetHuman(nullptr);
			}
			if (human->GetID() == 0) {
				gameCamera_->StartShake(1.5f, 4);
			}

		}
	}
}

void CheckCollision::CheckEnergy(Human* human) {
	for (auto& energy : course_->GetEnergies()) {
		Sphere energySphere = energy->GetCollider();
		Vector3 humanPos = human->GetTransform().translate;
		Sphere humanSphere = { humanPos, 1.0f };
		if (!energy->IsCoolDown(human->GetID())) { // 連続で触れられない
			// 判定
			if (IsCollision(energySphere, humanSphere)) {
				// 衝突
				energy->OnCollide(human->GetID());
				human->OnHitEnergy(energy->GetHealAmount());
			}
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
				human->OnHitWall(wall);
			}
		}
	}
}

void CheckCollision::CheckNeck(Human* human) {
	if (human->IsInvincible()) { return; }
	//if (human->isDrifting_ && human->IsCoilAround()) { return; } // 巻きつき中は判定しない
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
						if (human->GetID() == 0) {
							gameCamera_->StartShake(1.5f, 4);
						}
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
		isGoal_ = true;
	}
}

void CheckCollision::CheckVacuum(Human* human) {
	if (human->IsVacuuming()) {
		Sphere vacuumSphere = human->GetVacuumSphere();

		// リング
		for (auto& ring : course_->GetRings()) {
			Vector3 ringCenter = ring->GetColliderCenter();
			float ringHeight = ring->GetColliderHeight();

			// 高さの判定
			if (fabsf(ringCenter.y - vacuumSphere.center.y) <= ringHeight / 2.0f + vacuumSphere.radius) {
				// 横の判定
				float ringRadius = ring->GetColliderRadius();
				if (Length(Vector2{ ringCenter.x, ringCenter.z } - Vector2{ vacuumSphere.center.x, vacuumSphere.center.z }) <= ringRadius + vacuumSphere.radius) {
					// 衝突
					ring->OnCollide(0);
					Vector3 vel = Vector3(0, 0, 0);
					Vector3 axis = Vector3(0, 1, 0); // Y軸回転
					float deltaTime = 1.0f / 60.0f;

					if(mixType_ == 0)ring->Move(Mix(ringCenter, vel, vacuumSphere.center, axis));
					if(mixType_ == 1)ring->Move(Mix2(ringCenter, vel, vacuumSphere.center, axis));
				}
			}
		}

		// 障害物
		for (auto& spike : course_->GetSpikes()) {
			Sphere spikeSphere = spike->GetCollider();

			// 判定
			if (IsCollision(spikeSphere, vacuumSphere)) {
				// 衝突
				spike->OnCollide();

				Vector3 vel = Vector3(0, 0, 0);
				Vector3 axis = Vector3(0, 1, 0); // Y軸回転
				float deltaTime = 1.0f / 60.0f;
				if (mixType_ == 0)spike->Move(Mix(spikeSphere.center, vel, vacuumSphere.center, axis));
				if (mixType_ == 1)spike->Move(Mix2(spikeSphere.center, vel, vacuumSphere.center, axis));
			}
		}

		// エネルギー
		for (auto& energy : course_->GetEnergies()) {
			Sphere energySphere = energy->GetCollider();
			// 判定
			if (IsCollision(energySphere, vacuumSphere)) {
				// 衝突
				Vector3 vel = Vector3(0, 0, 0);
				Vector3 axis = Vector3(0, 1, 0); // Y軸回転
				float deltaTime = 1.0f / 60.0f;
				energy->Move(Mix2(energySphere.center, vel, vacuumSphere.center, axis));
				if (mixType_ == 0)energy->Move(Mix(energySphere.center, vel, vacuumSphere.center, axis));
				if (mixType_ == 1)energy->Move(Mix2(energySphere.center, vel, vacuumSphere.center, axis));
			}
		}
	}
}
