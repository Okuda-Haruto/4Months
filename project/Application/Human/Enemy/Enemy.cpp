#include "Enemy.h"
#include <numbers>

void Enemy::Initialize(Vector3 position, const std::shared_ptr<DirectionalLight> directionalLight) {
	//初期化
	Human::Initialize(position, directionalLight);
}

void Enemy::Update() {

	//速度などを加算する
	Human::Update();
}

void Enemy::Draw() {
	Human::Draw();
}