#include "Stars.h"
#include "HUD/HUD.h"
#include <numbers>

void Stars::Initialize(std::shared_ptr<Camera> camera) {
	camera_ = camera;

	for (int i = 0; i < kMaxStarCount; ++i) {
		std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
		sprite->Initialize("./resources/Effect/Star/Star.png");
		sprite->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
		sprite->SetSize(size_);
		sprite->SetAnchorPoint({ 0.5f,0.5f });
		sprite->Update();

		Star star;
		star.sprite = std::move(sprite);
		stars_.push_back(std::move(star));
	}

	for (int i = 0; i < kMaxFlashCount; ++i) {
		std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
		sprite->Initialize("./resources/Effect/Star/Star.png");
		sprite->SetColor({ 1.0f, 1.0f, 0.3f, 1.0f });
		sprite->SetSize(size_);
		sprite->SetAnchorPoint({ 0.5f,0.5f });
		sprite->Update();

		Flash flash;
		flash.sprite = std::move(sprite);
		flashes_.push_back(std::move(flash));
	}
}

void Stars::Update(Vector2 endPos) {
	for (int i = 0; i < kMaxStarCount; ++i) {
		if (isStarActive[i]) {
			if (stars_[i].t < 0.3f) {
				stars_[i].pos.x += stars_[i].velocity.x;
				stars_[i].pos.y += stars_[i].velocity.y;
			} else {
				float localT = (stars_[i].t - 0.3f) / 0.7f;
				float eased = localT * localT * localT;

				stars_[i].pos = Lerp(stars_[i].pos, endPos, eased);
			}

			if (stars_[i].t >= 1.0f) {
				isStarActive[i] = false;
				AddFlash(stars_[i].pos);
				AddFlash(stars_[i].pos);
			}

			stars_[i].t += 1.0f / 60.0f;
			stars_[i].sprite->SetPosition(stars_[i].pos);
			stars_[i].sprite->Update();
		}
	}

	for (int i = 0; i < kMaxFlashCount; ++i) {
		if (isFlashActive[i]) {
			flashes_[i].t += 2 * (1.0f / 60.0f);

			flashes_[i].pos.x += flashes_[i].velocity.x;
			flashes_[i].pos.y += flashes_[i].velocity.y;
			flashes_[i].sprite->SetPosition(flashes_[i].pos);
			flashes_[i].sprite->Update();

			Vector4 color = flashes_[i].sprite->GetColor();
			color.w = clamp(0.5f - flashes_[i].t / 2.0f, 0.0f, 1.0f);
			flashes_[i].sprite->SetColor(color);

			if (flashes_[i].t >= 1.0f) {
				isFlashActive[i] = false;
			}
		}
	}

}

void Stars::Draw() {
	for (int i = 0; i < kMaxStarCount; ++i) {
		if (isStarActive[i]) {
			stars_[i].sprite->Draw2D();
		}
	}

	for (int i = 0; i < kMaxFlashCount; ++i) {
		if (isFlashActive[i]) {
			flashes_[i].sprite->Draw2DAdd();
		}
	}
}

void Stars::AddStar(Vector3 worldPos) {
	for (int i = 0; i < kMaxStarCount; ++i) {
		if (!isStarActive[i]) {
			stars_[i].pos = ToScreen(camera_, worldPos);
			stars_[i].t = 0;

			// 初期速度をランダム方向に
			float angle = GameEngine::randomFloat(0.0f, 2.0f * float(std::numbers::pi));
			Vector2 dir;
			dir.x = cos(angle);
			dir.y = sin(angle);
			float speed = GameEngine::randomFloat(4.0f, 8.0f);
			stars_[i].velocity = dir * speed;
			isStarActive[i] = true;
			return;
		}
	}
}

void Stars::AddFlash(Vector2 screenPos) {
	for (int i = 0; i < kMaxFlashCount; ++i) {
		if (!isFlashActive[i]) {
			flashes_[i].pos = screenPos;
			flashes_[i].t = 0;
			// 初期速度をランダム方向に
			float angle = GameEngine::randomFloat(0.0f, 2.0f * float(std::numbers::pi));
			Vector2 dir;
			dir.x = cos(angle);
			dir.y = sin(angle);
			float speed = GameEngine::randomFloat(0.2f, 0.8f);
			flashes_[i].velocity = dir * speed;
			isFlashActive[i] = true;
			break;
		}
	}
}
