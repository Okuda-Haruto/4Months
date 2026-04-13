#pragma once
#include "GameEngine.h"
class Stars {
public:
	void Initialize(std::shared_ptr<Camera> camera);
	void Update(Vector2 endPos);
	void Draw();

	void AddStar(Vector3 worldPos);
	void AddFlash(Vector2 screenPos);
private:
	std::shared_ptr<Camera> camera_ = nullptr;

	struct Star {
		Vector2 pos;
		Vector2 velocity;
		float t = 0;
		std::unique_ptr<Sprite> sprite;
	};
	std::vector<Star> stars_;
	Vector2 size_ = {30,30};
	static const int kMaxStarCount = 100;
	bool isStarActive[kMaxStarCount]{};

	struct Flash {
		Vector2 pos;
		Vector2 velocity;
		float t = 0;
		std::unique_ptr<Sprite> sprite;
	};
	std::vector<Flash> flashes_;
	static const int kMaxFlashCount = 100;
	bool isFlashActive[kMaxFlashCount]{};
};

