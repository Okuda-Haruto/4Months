#include "TitleScreen.h"
#include <GameEngine.h>

void TitleScreen::Initialize(std::shared_ptr<GameCamera> gameCamera) {
	gameCamera_ = gameCamera;

	screen_Black_Sprite_ = std::make_unique<Sprite>();
	screen_Black_Sprite_->Initialize("./resources/DebugResources/white2x2.png");
	screen_Black_Sprite_->SetColor(Vector4{ 0,0,0,1 });
	screen_Black_Sprite_->SetSize(kScreeSize);

	screen_White_Sprite_ = std::make_unique<Sprite>();
	screen_White_Sprite_->Initialize("./resources/DebugResources/white2x2.png");
	screen_White_Sprite_->SetSize(Vector2{500,500});
	screen_White_Sprite_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
	screen_White_Sprite_->SetPosition(Vector2{ 640,360 });

	noiseColorA_ = 0;
	noiseUVTransform_ = {
		{16.0f,9.0f,1},
		{0,0,0},
		{0,0,0},
	};

	screen_Noise_Sprite_ = std::make_unique<Sprite>();
	screen_Noise_Sprite_->Initialize("./resources/Title/Noise.png");
	screen_Noise_Sprite_->SetSize(kScreeSize);
	screen_Noise_Sprite_->SetUVTransform(noiseUVTransform_);
	screen_Noise_Sprite_->SetColor(Vector4{ 1,1,1,noiseColorA_ });

	screenWhiteRate_ = { 0.1f,0.01f };

	isClearTime_ = kMaxIsClearTime_;
	titleTimer_ = 0.0f;

	noiseSE_ = make_unique<Audio>();
	noiseSE_->Initialize("resources/SE・BGM/Title/noise.mp3", 0.3f);
}

void TitleScreen::Update() {

	if (titleTimer_ < kMaxIsTitleTimer_) {
		titleTimer_ += GameEngine::GetDeltaTime();
	}

	if (titleTimer_ >= 1.0f) {
		if (titleTimer_ >= kMaxIsTitleTimer_ - 1.0f && !isCameraMove_) {
			gameCamera_->ChangeCamera(std::make_unique<TVCamera>(), 1.0f);
			isCameraMove_ = true;
		}
		if (titleTimer_ >= kMaxIsTitleTimer_) {
			isClear_ = true;
		}

		if (!isClear_) {
			if (screenWhiteRate_.x < 1.0f) {
				screenWhiteRate_.x += 0.15f * GameEngine::GetDeltaTimeRate();
				if (screenWhiteRate_.x > 1.0f) {
					screenWhiteRate_.x = 1.0f;
				}
			}
			else if (screenWhiteRate_.y < 1.0f) {
				screenWhiteRate_.y += 0.1f * GameEngine::GetDeltaTimeRate();
				if (screenWhiteRate_.y > 1.0f) {
					screenWhiteRate_.y = 1.0f;
				}
			}
			else {
				if (noiseColorA_ == 0) {
					noiseSE_->SoundPlayWave();
				}

				noiseColorA_ += 0.1f * GameEngine::GetDeltaTimeRate();
				if (noiseColorA_ > 1.0f) {
					noiseColorA_ = 1.0f;
				}
				screen_Noise_Sprite_->SetColor(Vector4{ 1,1,1,noiseColorA_ });
			}
		}
		else if (!isEnd_) {
			isClearTime_ -= GameEngine::GetDeltaTime();

			if (isClearTime_ < 0.0f) {
				isClearTime_ = 0.0f;
			}

			noiseColorA_ = isClearTime_ / kMaxIsClearTime_;

			screen_White_Sprite_->SetColor(Vector4{ 1,1,1,0 });
			screen_Black_Sprite_->SetColor(Vector4{ 0,0,0,0 });
			screen_Noise_Sprite_->SetColor(Vector4{ 1,1,1,noiseColorA_ });

			if (isClearTime_ <= 0.0f) {
				isEnd_ = true;
				noiseSE_->SoundEndWave();
			}
		}
		screen_White_Sprite_->SetSize(Vector2{ kScreeSize.x * screenWhiteRate_.x,kScreeSize.y * screenWhiteRate_.y });
		noiseUVTransform_.translate = { GameEngine::randomFloat(-128,128),GameEngine::randomFloat(-128,128) ,0 };

		screen_Noise_Sprite_->SetUVTransform(noiseUVTransform_);

		screen_White_Sprite_->Update();

		screen_Noise_Sprite_->Update();
	}

	screen_Black_Sprite_->Update();
}

void TitleScreen::Draw() {
	screen_Black_Sprite_->Draw2D();
	screen_White_Sprite_->Draw2D();
	screen_Noise_Sprite_->Draw2D();
}