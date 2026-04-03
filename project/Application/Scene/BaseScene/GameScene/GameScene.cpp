#define NOMINMAX
#include "GameScene.h"
#include "../../SceneManager/SceneManager.h"

#include <algorithm>
#include <numbers>
#include <cmath>

using namespace std;

namespace {
	bool sWasAchieved = false;
}

void GameScene::Initialize(std::shared_ptr<Input> input) {

	input_ = input;

	ModelManager::GetInstance()->LoadModel("resources/DebugResources/sphere", "sphere.obj");

	//デバッグカメラ
	debugCamera_ = make_shared<DebugCamera>();
	debugCamera_->Initialize(input_);

	//カメラ
	defaultCamera_ = Object::GetDefaultCamera();
	defaultCamera_->SetDebugCamera(debugCamera_);

	//光源
	directionalLight_ = make_shared<DirectionalLight>();
	directionalLight_->Initialize(GameEngine::GetDirectXCommon());
	directionalLightElement_ = {
		{1.0f,1.0f,1.0f,1.0f},
		Normalize(Vector3{0.0f,-1.0f,0.25f}),
		1.0f
	};
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	//ゴール
	goal_ = std::make_unique<Goal>();
	goal_->Initialize(Vector3{ 0,-500,0 }, directionalLight_);

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(Vector3{ 0,200,0 }, directionalLight_);

	//カメラ
	gameCamera_ = make_unique<GameCamera>();
	gameCamera_->Initialize(defaultCamera_, input_, player_.get());

	// コース
	course_ = std::make_unique<Course>();
	course_->Initialize(gameCamera_.get(), directionalLight_);
	chunkHeight_ = int(course_->GetChunkData().size.y);

	// 当たり判定
	checkCollision_ = std::make_unique<CheckCollision>();
	checkCollision_->Initialize(course_.get(), goal_.get(), gameCamera_.get());

	// HUD
	hud_ = std::make_unique<HUD>();
	hud_->Initialize(input.get());

	// 予測表示
	hitPreview_ = std::make_unique<HitPreview>();
	hitPreview_->Initialize(directionalLight_);

	// 残り時間
	timer_ = std::make_unique<GameTimer>();
	timer_->Initialize();

	startTime_ = kMamStartTime;
	hud_->ResetGoal();
	sWasAchieved = false;
	clearCount_ = 0;
#ifdef USE_IMGUI
	isUseDebugCamera_ = false;
#endif

}

void GameScene::Finalize() {

}

void GameScene::Update() {
	Keyboard keyboard = input_->GetKeyBoard();
	Pad pad = input_->GetPad(0);

	float clearBorderY = -32 * chunkHeight_ * 3.0f;
	bool isReachedClearY = player_->GetTransform().translate.y < clearBorderY;

	if (startTime_ > 0.0f) {
		startTime_ -= 2.0f / 60.0f;
	}
	else {
		// プレイヤーの更新
		player_->Update(input_);

		if (isClear_) {
			if (isUp_) {
				clearY_ += 1.0f;
				if (clearY_ > 0) {
					isUp_ = false;
				}
			}
			else {
				clearY_ -= 1.0f;
				if (clearY_ < -32 * 3.0f * 4) {
					isUp_ = true;
				}
			}

			if (keyboard.trigger[DIK_SPACE] || pad.Button[PAD_BUTTON_B].trigger) {
				SceneManager::GetInstance()->ChangeScene("Title");
				return;
			}
		}

		goal_->Update();
		course_->Update();

		checkCollision_->Update(player_.get());
		checkCollision_->UpdateImGui();

		hitPreview_->Update(player_.get(), checkCollision_.get());

		timer_->Update();

		clearBorderY = -32 * chunkHeight_ * 3.0f;
		isReachedClearY = player_->GetTransform().translate.y < clearBorderY;

		// =========================
		// ■ 制限時間切れ
		// =========================
		if (timer_->GetCurrentGameTime() <= 0.0f) {
			if (!isReachedClearY) {
				SceneManager::GetInstance()->ChangeScene("Title");
				return;
			}
		}
	}

	gameCamera_->Update();

	bool nowAchieved = course_->GetBreakScore() >= course_->GetMaxBreakScore();

	if (nowAchieved && !sWasAchieved) {
		hud_->OnGoalAchieved();
	}

	sWasAchieved = nowAchieved;

	if (isUseDebugCamera_) {
		defaultCamera_->Update();
	}
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	hud_->Update(player_.get(), course_.get(), timer_.get(), int(std::ceil(startTime_)));
#ifdef USE_IMGUI
	int score = course_->GetBreakScore();
	int max = course_->GetMaxBreakScore();
	ImGui::Begin("GameScene");
	if (ImGui::Button("デバッグカメラ")) {
		isUseDebugCamera_ = !isUseDebugCamera_;
	}
	ImGui::Text("スコア :%6d", score);
	ImGui::Text("ノルマ :%6d", max);
	ImGui::End();
#endif

	if (keyboard.trigger[DIK_R]) {
		SceneManager::GetInstance()->ChangeScene("Game");
		return;
	}

	// =========================
	// ■ クリアY到達
	// =========================
	if (player_->GetTransform().translate.y < -32 * chunkHeight_ * 3.0f) {

		if (course_->GetBreakScore() >= course_->GetMaxBreakScore()) {

			timer_->Initialize();
			clearCount_++;   // ★追加
			// ★追加ここ
			startTime_ = kMamStartTime;
			course_->Initialize(gameCamera_.get(), directionalLight_);
			chunkHeight_ = int(course_->GetChunkData().size.y);

			checkCollision_->Initialize(course_.get(), goal_.get(), gameCamera_.get());

			player_->Initialize(Vector3{ 0,200,0 }, directionalLight_);

			hud_->ResetGoal();
			sWasAchieved = false;

		}
		else {
			SceneManager::GetInstance()->ChangeScene("Title");
			return;
		}
	}

}

void GameScene::Draw() {

	if (isClear_) {
		course_->DrawAll(directionalLight_);
	}
	else {
		player_->Draw();
		course_->Draw(directionalLight_);
		hitPreview_->Draw();
		hud_->Draw();
	}

}