#pragma once
#include "GameEngine.h"
class Stars {
public:
	void Initialize(std::shared_ptr<Camera> camera);
	void Update(Vector2 endPos);
	void Draw();

	void AddStar(Vector3 worldPos);
private:
	std::shared_ptr<Camera> camera_ = nullptr;

	struct Star {
		Vector2 pos;
		Vector2 velocity;
		float t = 0;
		std::unique_ptr<Sprite> sprite;
	};
	std::vector<Star> stars_;
	Vector2 size_ = {20,20};
	static const int kMaxCount = 100;
	bool isActive[kMaxCount]{};
};

