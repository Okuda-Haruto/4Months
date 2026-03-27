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
	player_->Initialize(Vector3{0,100,0}, directionalLight_);
	player_->SetGoal(goal_.get());

	player_->SetSelfNeckIndex(0);

	//カメラ
	gameCamera_ = make_unique<GameCamera>();
	gameCamera_->Initialize(defaultCamera_, player_.get());

	// コース
	course_ = std::make_unique<Course>();
	course_->Initialize(gameCamera_.get(), directionalLight_);

	// 当たり判定
	checkCollision_ = std::make_unique<CheckCollision>();
	checkCollision_->Initialize(course_.get(),goal_.get(),gameCamera_.get());

	// HUD
	hud_ = std::make_unique<HUD>();
	hud_->Initialize();

#ifdef USE_IMGUI
	isUseDebugCamera_ = false;
#endif

}

void GameScene::Finalize() {
	
}

void GameScene::Update() {
	Keyboard keyboard = input_->GetKeyBoard();

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
		clearCameraTransform_.rotate = clearCameraTransform_.rotate * MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 180);
		clearCameraTransform_.translate = RotateVector({ 0,0,-16 * 3 - 300 }, Inverse(clearCameraTransform_.rotate));
		clearCameraTransform_.translate.y = clearY_;
		defaultCamera_->Update(clearCameraTransform_);

		//クリアしてるならタイトルに戻れる
		if (keyboard.trigger[DIK_SPACE]) {
			SceneManager::GetInstance()->ChangeScene("Title");
		}

	} else {
		//カメラ更新
		gameCamera_->Update();
	}

	//ゴール更新処理
	goal_->Update();

	// コース
	course_->Update();

	// 当たり判定
	checkCollision_->Update(player_.get());
	checkCollision_->UpdateImGui();

	// HUD
	hud_->Update(player_.get(),course_.get());

	//カメラアップデート
	if (isUseDebugCamera_) {
		defaultCamera_->Update();
	}
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

#ifdef USE_IMGUI
	ImGui::Begin("GameScene");
	if (ImGui::Button("デバッグカメラ")) {
		isUseDebugCamera_ = !isUseDebugCamera_;
	}
	ImGui::End();
#endif

	//仮置き
	if (keyboard.trigger[DIK_R]) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
	if (checkCollision_->IsGoal()) {
		if (!isClear_) {
			clearCameraTransform_.translate = { 0,-16 * 3 * 2,-16 * 3 - 300 };
			clearCameraTransform_.rotate = IdentityQuaternion();
			clearCameraTransform_.scale = { 1,1,1 };
		}
		isClear_ = true;
		//SceneManager::GetInstance()->ChangeScene("Result");
	}

}

void GameScene::Draw() {
	//描画処理
	player_->Draw();

	//ゴール描画処理
	goal_->Draw();

	// コース
	if (isClear_) {
		course_->DrawAll(directionalLight_);
	} else {
		course_->Draw(directionalLight_);
	}

	// HUD
	hud_->Draw();

}