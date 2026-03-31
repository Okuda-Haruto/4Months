#define NOMINMAX
#include "GameScene.h"
#include "../../SceneManager/SceneManager.h"

#include <algorithm>
#include <numbers>
#include <cmath>

using namespace std;

void GameScene::Initialize(std::shared_ptr<Input> input) {

	input_ = input;

	ModelManager::GetInstance()->LoadModel("resources/DebugResources/sphere", "sphere.obj");

	//デバッグカメラ
	debugCamera_ = make_shared<DebugCamera>();
	debugCamera_->Initialize(input_);

	//カメラ
	defaultCamera_ = Object::GetDefaultCamera();
	//defaultCamera_ = make_shared<Camera>();
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
	player_->Initialize(Vector3{0,200,0}, directionalLight_);

	//カメラ
	gameCamera_ = make_unique<GameCamera>();
	gameCamera_->Initialize(defaultCamera_,input_ , player_.get());

	// コース
	course_ = std::make_unique<Course>();
	course_->Initialize(gameCamera_.get(), directionalLight_);
	chunkHeight_ = int(course_->GetChunkData().size.y);

	// 当たり判定
	checkCollision_ = std::make_unique<CheckCollision>();
	checkCollision_->Initialize(course_.get(),goal_.get(),gameCamera_.get());

	// HUD
	hud_ = std::make_unique<HUD>();
	hud_->Initialize();

	// 予測表示
	hitPreview_ = std::make_unique<HitPreview>();
	hitPreview_->Initialize(directionalLight_);

	// 残り時間
	timer_ = std::make_unique<GameTimer>();
	timer_->Initialize();

#ifdef USE_IMGUI
	isUseDebugCamera_ = false;
#endif

}

void GameScene::Finalize() {
	
}

void GameScene::Update() {
	Keyboard keyboard = input_->GetKeyBoard();
	Pad pad = input_->GetPad(0);

	// プレイヤーの更新
	player_->Update(input_);

	if (isClear_) {
		if (isUp_) {
			clearY_ += 1.0f;
			if (clearY_ > 0) {
				isUp_ = false;
			}
		} else {
			clearY_ -= 1.0f;
			if (clearY_ < -32 * 3.0f * 4) {
				isUp_ = true;
			}
		}

		//クリアしてるならタイトルに戻れる
		if (keyboard.trigger[DIK_SPACE] || pad.Button[PAD_BUTTON_B].trigger) {
			SceneManager::GetInstance()->ChangeScene("Title");
		}

	}
	//カメラ更新
	gameCamera_->Update();

	//ゴール更新処理
	goal_->Update();

	// コース
	course_->Update();

	// 当たり判定
	checkCollision_->Update(player_.get());
	checkCollision_->UpdateImGui();

	// HUD
	hud_->Update(player_.get(),course_.get(),timer_.get());
	timer_->Update();

	// 予測表示
	hitPreview_->Update(player_.get(),checkCollision_.get());

	//カメラアップデート
	if (isUseDebugCamera_) {
		defaultCamera_->Update();
	}
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

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

	//仮置き
	if (keyboard.trigger[DIK_R]) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}

	if (player_->GetTransform().translate.y < -32 * chunkHeight_ * 3.0f) {
		if (course_->GetBreakScore() >= course_->GetMaxBreakScore()) {
			if (!isClear_) {
				clearCameraTransform_.translate = { 0,-16 * 3 * 2,-16 * 3 - 300 };
				clearCameraTransform_.rotate = IdentityQuaternion();
				clearCameraTransform_.scale = { 1,1,1 };
				gameCamera_->ChangeCamera(std::make_unique<ResultCamera>(), 1.0f);
			}
			isClear_ = true;
		} else {
			SceneManager::GetInstance()->ChangeScene("Title");
		}
	}

}

void GameScene::Draw() {

	// コース
	if (isClear_) {
		course_->DrawAll(directionalLight_);
	} else {
		//描画処理
		player_->Draw();

		//ゴール描画処理
		//goal_->Draw();

		course_->Draw(directionalLight_);

		hitPreview_->Draw();

		// HUD
		hud_->Draw();
	}

}