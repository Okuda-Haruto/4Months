#pragma once
#include <Sprite/Sprite.h>
#include <Object/Object.h>
#include <GameCamera/GameCamera.h>
#include <array>

class GameOver {
private:

	std::unique_ptr<Sprite> flowerGarden_Sprite_;
	std::unique_ptr<Sprite> videoDistorted_Sprite_;
	std::unique_ptr<Sprite> pressToNext_Sprite_;
	std::unique_ptr<Sprite> normaOver_Sprite_;

	std::array<std::unique_ptr<Object>,4> butterfly_;

	//専用のカメラ
	std::shared_ptr<Camera> camera_;

	float eventTime_;
	const float kMaxEventTime_ = 4.0f;

	float plessToNextColor_;

public:
	//初期化
	void Initialize(std::shared_ptr<DirectionalLight> directionalLight);
	//更新処理
	void Update();
	//描画処理
	void Draw();

	void Reset() { eventTime_ = 0.0f; plessToNextColor_ = 0; }
};