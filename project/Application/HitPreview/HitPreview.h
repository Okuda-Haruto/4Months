#pragma once
#include "GameEngine.h"
class Player;
class CheckCollision;

class HitPreview {
public:
	void Initialize(const std::shared_ptr<DirectionalLight> directionalLight);
	void Update(Player* player, CheckCollision* checkCollision);
	void Draw();

private:
	void Simulate(Player* player, CheckCollision* checkCollision);

	bool canDraw_ = false;
	Vector3 hitPos_{};
	std::unique_ptr<Object> circle_ = nullptr;
	std::vector<std::unique_ptr<Object>> radModel_;

	int rotateCount_ = 8;
	float rotateSpeed_ = 0.2f;
	std::vector<float> radRotate_;
};

