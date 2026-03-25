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
