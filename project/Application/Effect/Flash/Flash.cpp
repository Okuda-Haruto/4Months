#include "Flash.h"

void Flash::Initialize() {
	flash_ = std::make_unique<Sprite>();
	flash_->Initialize("resources/DebugResources/white2x2.png");

	flash_->SetColor({});
	flash_->SetAnchorPoint({ 0.5f,0.5f });
	flash_->SetSize(Vector2{ float(GameEngine::GetWindowWidth()), float(GameEngine::GetWindowHeight()) });
	flash_->SetPosition(flash_->GetSize() / 2.0f);
}

void Flash::Update() {
	if (isStart_) {
		timer_ += GameEngine::GetDeltaTime();

		float t = 0;
		float max = time_ * 0.3f;
		Vector4 color = { 1,1,1,0 };
		if (timer_ < max) {
			t = std::clamp(timer_ / max, 0.0f, 1.0f);
			color.w = t;
		} else {
			t = std::clamp((timer_ - max) / (time_ - max), 0.0f, 1.0f);
			color.w = 1 - t;
		}
		flash_->SetColor(color);

		if (timer_ > time_) isStart_ = false;
	}

	flash_->Update();
}

void Flash::Draw() {
	flash_->Draw2D();
}

void Flash::Set() {
	flash_->SetColor({});
	isStart_ = true;
	timer_ = 0;
}
