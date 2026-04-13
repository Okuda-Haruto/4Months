#include "Stars.h"
#include "HUD/HUD.h"
#include <numbers>

// ベジェ曲線(Vector2)
static Vector2 Bezier(Vector2 p0, Vector2 p1, Vector2 p2, float t) {
	float u = 1.0f - t;
	return u * u * p0 + 2.0f * u * t * p1 + t * t * p2;
}

// easeOut（吸い込み強調）
static float EaseOutCubic(float t) {
	return 1.0f - powf(1.0f - t, 3.0f);
}

void Stars::Initialize(std::shared_ptr<Camera> camera) {
	camera_ = camera;

	for (int i = 0; i < kMaxStarCount; ++i) {
		std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
		sprite->Initialize("./resources/Effect/Star/Star.png");
		sprite->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
		sprite->SetSize(size_);
		sprite->SetAnchorPoint({ 0.5f, 0.5f });
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
		sprite->SetAnchorPoint({ 0.5f, 0.5f });
		sprite->Update();

		Flash flash;
		flash.sprite = std::move(sprite);
		flashes_.push_back(std::move(flash));
	}
}

void Stars::Update(Vector2 endPos) {
	for (int i = 0; i < kMaxStarCount; ++i) {
		if (!isStarActive[i]) continue;

		float t = stars_[i].t;
		float eased = EaseOutCubic(t);

		// ベジェで位置更新
		stars_[i].pos = Bezier(
			stars_[i].start,
			stars_[i].control,
			endPos,
			eased
		);

		// 到達
		if (t >= 0.8f) {
			isStarActive[i] = false;

			AddFlash(stars_[i].pos);
			AddFlash(stars_[i].pos);
		}

		stars_[i].t += 1.0f / 60.0f;

		stars_[i].sprite->SetPosition(stars_[i].pos);
		stars_[i].sprite->SetRotation(stars_[i].sprite->GetRotation() - stars_[i].rotateSpeed);
		stars_[i].sprite->Update();
	}

	// フラッシュ
	for (int i = 0; i < kMaxFlashCount; ++i) {
		if (!isFlashActive[i]) continue;
		flashes_[i].t += kFlashLifetime * (1.0f / 60.0f);

		flashes_[i].pos = flashes_[i].pos + flashes_[i].velocity;

		flashes_[i].sprite->SetPosition(flashes_[i].pos);
		flashes_[i].sprite->SetRotation(flashes_[i].sprite->GetRotation() - flashes_[i].rotateSpeed);
		flashes_[i].sprite->Update();

		Vector4 color = flashes_[i].sprite->GetColor();
		color.w = clamp(0.5f - flashes_[i].t / (kFlashLifetime * 2), 0.0f, 1.0f);
		flashes_[i].sprite->SetColor(color);

		if (flashes_[i].t >= 1.0f) {
			isFlashActive[i] = false;
		}
	}

#ifdef USE_IMGUI
	int starCount = 0;
	int flashCount = 0;
	for (int i = 0; i < kMaxStarCount; ++i) {
		if (isStarActive[i]) starCount++;
	}
	for (int i = 0; i < kMaxFlashCount; ++i) {
		if (isFlashActive[i]) flashCount++;
	}

	ImGui::Begin("Effect");
	ImGui::Text("StarCount: %d / %d", starCount,kMaxStarCount);
	ImGui::Text("FlashCount: %d / %d", flashCount,kMaxFlashCount);
	ImGui::End();
#endif
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
		if (isStarActive[i]) continue;

		Vector2 start = ToScreen(camera_, worldPos);

		// ランダム方向
		float angle = GameEngine::randomFloat(0.0f, 2.0f * float(std::numbers::pi));
		Vector2 dir = { cosf(angle), sinf(angle) };

		float speed = GameEngine::randomFloat(6.0f, 12.0f);

		// 制御点
		Vector2 control = start + dir * speed * 20.0f;
		control.y += 50;

		stars_[i].start = start;
		stars_[i].control = control;
		stars_[i].t = 0.0f;
		stars_[i].rotateSpeed = 0.3f;

		isStarActive[i] = true;
		return;
	}
}

void Stars::AddFlash(Vector2 screenPos) {
	for (int i = 0; i < kMaxFlashCount; ++i) {
		if (isFlashActive[i]) continue;

		flashes_[i].pos = screenPos;
		flashes_[i].t = 0;
		flashes_[i].sprite->SetRotation(GameEngine::randomFloat(0.0f, 2.0f * float(std::numbers::pi)));
		flashes_[i].rotateSpeed = GameEngine::randomFloat(-0.1f, 0.1f);

		float angle = GameEngine::randomFloat(0.0f, 2.0f * float(std::numbers::pi));
		Vector2 dir = { cosf(angle), sinf(angle) };

		float speed = GameEngine::randomFloat(0.2f, 0.8f);
		flashes_[i].velocity = dir * speed;

		isFlashActive[i] = true;
		break;
	}
}