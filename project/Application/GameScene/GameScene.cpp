#define NOMINMAX
#include "GameScene.h"
#include "Vector3.h"
#include "Matrix4x4.h"

#include <algorithm>
#include <numbers>
#include <cmath>

using namespace std;

void GameScene::Initialize() {

	input_ = make_shared<Input>();
	input_->Initialize(GameEngine::GetWindowsAPI());

	ModelManager::GetInstance()->LoadModel("resources/DebugResources/sphere", "sphere.obj");

	//デバッグカメラ
	debugCamera_ = make_shared<DebugCamera>();
	debugCamera_->Initialize(input_);

	//カメラ
	defaultCamera_ = Object::GetDefaultCamera();
	defaultCamera_->SetDebugCamera(debugCamera_);

	cameraTransform_ = {
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 2.0f, 2.0f, -18.0f }
	};

	//光源
	directionalLight_ = make_shared<DirectionalLight>();
	directionalLight_->Initialize(GameEngine::GetDirectXCommon());
	directionalLightElement_ = {
		{1.0f,1.0f,1.0f,1.0f},
		{0.0f,-1.0f,0.0f},
		1.0f
	};
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	player_ = std::make_unique<Player>();
	player_->Initialize(directionalLight_);

	neck_ = std::make_unique<Neck>();
	neck_->Initialize(player_.get(), directionalLight_);

	// コース
	course_ = std::make_unique<Course>();
	course_->Initialize();

#ifdef USE_IMGUI
	isUseDebugCamera_ = true;
#endif

}

void GameScene::Update() {
	//入力処理
	input_->Update();

	// プレイヤーの更新
	player_->Update(input_);

	neck_->Update();

	// コース
	course_->Update();

	//カメラアップデート
	if (isUseDebugCamera_) {
		defaultCamera_->Update();
	}
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);
}

void GameScene::Draw() {
	//描画処理
	player_->Draw();

	neck_->Draw();

	// コース
	course_->Draw(directionalLight_);
}