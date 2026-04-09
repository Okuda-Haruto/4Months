#include "Stars.h"
#include "HUD/HUD.h"
#include <numbers>

void Stars::Initialize(std::shared_ptr<Camera> camera) {
	camera_ = camera;
	for (int i = 0; i < kMaxCount; ++i) {
		std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
		sprite->Initialize("./resources/DebugResources/white2x2.png");
		sprite->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
		sprite->SetSize(size_);
		sprite->SetAnchorPoint({ 0.5f,0.5f });
		sprite->Update();

		Star star;
		star.sprite = std::move(sprite);
		stars_.push_back(std::move(star));
	}
}

void Stars::Update(Vector2 endPos) {
	for (int i = 0; i < kMaxCount; ++i) {
		if (isActive[i]) {
			if (stars_[i].t < 0.3f) {
				stars_[i].pos.x += stars_[i].velocity.x;
				stars_[i].pos.y += stars_[i].velocity.y;
			} else {
				float localT = (stars_[i].t - 0.3f) / 0.7f;
				float eased = localT * localT * localT;

				stars_[i].pos = Lerp(stars_[i].pos, endPos, eased);
			}

			if (stars_[i].t >= 1.0f) {
				isActive[i] = false;
			}

			stars_[i].t += 1.0f / 60.0f;
			stars_[i].sprite->SetPosition(stars_[i].pos);
			stars_[i].sprite->Update();
		}
	}
}

void Stars::Draw() {
	for (int i = 0; i < kMaxCount; ++i) {
		if (isActive[i]) {
			stars_[i].sprite->Draw2D();
		}
	}
}

void Stars::AddStar(Vector3 worldPos) {
	for (int i = 0; i < kMaxCount; ++i) {
		if (!isActive[i]) {
			stars_[i].pos = ToScreen(camera_, worldPos);
			stars_[i].t = 0;

			float angle = GameEngine::randomFloat(0.0f, 2.0f * float(std::numbers::pi));
			Vector2 dir;
			dir.x = cos(angle);
			dir.y = sin(angle);
			float speed = GameEngine::randomFloat(2.0f, 4.0f);
			stars_[i].velocity = dir * speed;
			isActive[i] = true;
			return;
		}
	}
}
