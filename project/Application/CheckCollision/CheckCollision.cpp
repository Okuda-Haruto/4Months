#include "CheckCollision.h"
#include "Player/Player.h"
#include "Course/Course.h"
#include "Math/Collision.h"

void CheckCollision::Initialize(Player* player, Course* course) {
	player_ = player;
	course_ = course;
}

void CheckCollision::Update() {
	CheckRing();
	CheckSpike();
	CheckWall();
}

void CheckCollision::CheckRing() {
	for (auto& ring : course_->GetRings()) {
		Vector3 ringCenter = ring->GetColliderCenter();
		float ringHeight = ring->GetColliderHeight();
		Vector3 playerPos = player_->GetTransform().translate;

		// 高さの判定
		if (fabsf(ringCenter.y - playerPos.y) <= ringHeight / 2.0f) {
			// 横の判定
			float ringRadius = ring->GetColliderRadius();
			Vector3 ringCenter2D = { ringCenter.x, ringCenter.z };
			if (Length(Vector2{ ringCenter.x, ringCenter.z } - Vector2{ playerPos.x, playerPos.z }) <= ringRadius) {
				// 衝突
				ring->OnCollide();
			}
		}
	}
}

void CheckCollision::CheckSpike() {
	for (auto& spike : course_->GetSpikes()) {
		Sphere spikeSphere = spike->GetCollider();
		Vector3 playerPos = player_->GetTransform().translate;
		Sphere playerSphere = {playerPos, 1.0f};

		// 判定
		if (IsCollision(spikeSphere, playerSphere)) {
			// 衝突
			spike->OnCollide();
		}
	}
}

void CheckCollision::CheckWall() {
	for (auto& wall : course_->GetWalls()) {
		Vector3 playerPos = player_->GetTransform().translate;
		Sphere playerSphere = { playerPos, 1.0f };

		// 判定
		if (IsCollision(wall, playerSphere)) {
			// 衝突
			course_->OnCollide();
		}
	}
}