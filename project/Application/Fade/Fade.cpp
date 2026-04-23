#include "Fade.h"

void Fade::Initialzie() {
	fadeTime_ = 0.0f;
	isEnd_ = true;
	isFirstFlame_ = true;

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("./resources/DebugResources/white2x2.png");
	sprite_->SetColor(Vector4{ 0,0,0,1 });
	sprite_->SetSize(Vector2{ 1280,720 });

}

void Fade::Update() {

	if (!isEnd_) {

		if (isFirstFlame_) {
			isFirstFlame_ = false;
		}
		else {
			fadeTime_ -= 1.0f / 60.0f;
		}

		if (fadeTime_ <= 0.0f) {
			fadeTime_ = 0.0f;
			isEnd_ = true;
		}

		switch (fadeMode_)
		{
		case Fade::FADE_MODE::FADE_IN:
			sprite_->SetColor(Vector4{ 0,0,0,fadeTime_ / kMaxFadeTime_ });
			break;
		case Fade::FADE_MODE::FADE_OUT:
			sprite_->SetColor(Vector4{ 0,0,0,1.0f - fadeTime_ / kMaxFadeTime_ });
			break;
		default:
			break;
		}

	}
	sprite_->Update();
}

void Fade::Draw() {
	sprite_->Draw2D();
}

void Fade::SetFadeMode(FADE_MODE fadeMode) {
	fadeMode_ = fadeMode;
	fadeTime_ = kMaxFadeTime_;
	isEnd_ = false;
	isFirstFlame_ = true;
}