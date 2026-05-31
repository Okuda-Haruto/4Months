#pragma once
#include <Sprite/Sprite.h>
#include <GameCamera/GameCamera.h>

class TitleScreen {
private:

	const Vector2 kScreeSize = { 1280,720 };

	Vector2 screenWhiteRate_ = { 0.0f,0.0f };
	float noiseColorA_;
	SRT noiseUVTransform_;

	//消すか
	bool isClear_ = false;
	//完全に消えたか
	bool isEnd_ = false;
	//カメラ移動をさせたか
	bool isCameraMove_ = false;

	const float kMaxIsClearTime_ = 0.5f;
	float isClearTime_;

	const float kMaxIsTitleTimer_ = 3.0f;
	float titleTimer_;

	//スプライト
	std::unique_ptr<Sprite> screen_Black_Sprite_;
	std::unique_ptr<Sprite> screen_White_Sprite_;
	std::unique_ptr<Sprite> screen_Noise_Sprite_;

	std::shared_ptr<GameCamera> gameCamera_;


	std::unique_ptr<Audio> noiseSE_ = nullptr;

public:

	//初期化
	void Initialize(std::shared_ptr<GameCamera> gameCamera);
	//更新処理
	void Update();
	//描画処理
	void Draw();

	bool IsClear() { return isClear_; }
	bool IsEnd() { return isEnd_; }

	void SetEnd() { isEnd_ = true; }
};