#include "CheckCollision.h"
#include "Human/Human.h"
#include "Course/Course.h"
#include "Goal/Goal.h"
#include "Math/Collision.h"
#include "GameCamera/GameCamera.h"
#include "Mix.h"

void CheckCollision::Initialize(Course* course, Goal* goal, GameCamera* gameCamera) {
	course_ = course;
	goal_ = goal;
	gameCamera_ = gameCamera;
}

void CheckCollision::Update(Human* human) {
	CheckSpike(human);
	CheckEnergy(human);
	CheckVoxel(human);
	CheckGoal(human);
	CheckVacuum(human);
}

void CheckCollision::UpdateImGui() {
#ifdef USE_IMGUI
		ImGui::Begin("かき混ぜ");
		ImGui::SliderInt("挙動", &mixType_, 0, 1);
		ImGui::End();
#endif
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

void CheckCollision::CheckVoxel(Human* human) {
	if (human->IsInvincible()) { return; }
	Vector3 playerPos = human->GetTransform().translate;
	Sphere playerSphere = { playerPos, 1.0f };

	//ボクセル
	course_->GetVoxel()->Collision(playerSphere);
	for (auto& box : course_->GetBoxes()) {
		Sphere boxSphere = box->GetCollider();

		// 判定
		if (IsCollision(boxSphere, playerSphere)) {
			// 衝突
			human->OnHitVoxel();
			
			if (human->GetID() == 0) {
				gameCamera_->StartShake(1.5f, 4);
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
				float length = Length(vacuumSphere.center - spikeSphere.center);
				//割合で速度を変える
				float vecuumLate = length / vacuumSphere.radius;
				//速度
				float speed = (1.0f - vecuumLate) * baseVacuumSpeed_;

				if (mixType_ == 0)spike->Move(Mix(spikeSphere.center, vel, vacuumSphere.center, axis) * speed);
				if (mixType_ == 1)spike->Move(Mix2(spikeSphere.center, vel, vacuumSphere.center, axis) * speed);
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

				float length = Length(vacuumSphere.center - energySphere.center);
				//割合で速度を変える
				float vecuumLate = length / vacuumSphere.radius;
				//速度
				float speed = (1.0f - vecuumLate) * baseVacuumSpeed_;

				if (mixType_ == 0)energy->Move(Mix(energySphere.center, vel, vacuumSphere.center, axis) * speed);
				if (mixType_ == 1)energy->Move(Mix2(energySphere.center, vel, vacuumSphere.center, axis) * speed);
			}
		}

		//ボクセル
		course_->GetVoxel()->Collision(vacuumSphere);

		//ボックス(吸い込まれてるボクセル)
		for (auto& box : course_->GetBoxes()) {
			Sphere boxSphere = box->GetCollider();
			// 判定
			if (IsCollision(boxSphere, vacuumSphere)) {
				// 衝突
				Vector3 vel = Vector3(0, 0, 0);
				Vector3 axis = Vector3(0, 1, 0); // Y軸回転
				float deltaTime = 1.0f / 60.0f;

				float length = Length(vacuumSphere.center - boxSphere.center);
				//割合で速度を変える
				float vecuumLate = length / vacuumSphere.radius;
				//速度
				float speed = (1.0f - vecuumLate) * baseVacuumSpeed_;

				if (mixType_ == 0)box->Move(Mix(boxSphere.center, vel, vacuumSphere.center, axis) * speed);
				if (mixType_ == 1)box->Move(Mix2(boxSphere.center, vel, vacuumSphere.center, axis) * speed);

				if (length < 17.0f) {
					box->Damage();
				}
			}
		}
	}
}
