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
		Normalize(Vector3{0.0f,-1.0f,0.25f}),
		1.0f
	};
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	// コース
	course_ = std::make_unique<Course>();
	course_->Initialize();
	//ゴール
	goal_ = std::make_unique<Goal>();
	goal_->Initialize(Vector3{ 0,-380,0 }, directionalLight_);

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(Vector3{0,100,0}, directionalLight_);
	player_->SetGoal(goal_.get());

	//相手
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(Vector3{ 0,100,10 }, directionalLight_);
	enemy_->SetRings(course_->GetRings());
	enemy_->SetSpikes(course_->GetSpikes());
	enemy_->SetGoal(goal_.get());

	//首
	std::unique_ptr<Neck> neck;
	neck = std::make_unique<Neck>();
	neck->Initialize(player_.get(), directionalLight_);
	necks_.push_back(move(neck));
	neck = std::make_unique<Neck>();
	neck->Initialize(enemy_.get(), directionalLight_);
	necks_.push_back(move(neck));
	player_->SetNeck(necks_[1].get());

	//カメラ
	gameCamera_ = make_unique<GameCamera>();
	gameCamera_->Initialize(defaultCamera_, player_.get());


	// 当たり判定
	checkCollision_ = std::make_unique<CheckCollision>();
	checkCollision_->Initialize(player_.get(), course_.get());

#ifdef USE_IMGUI
	isUseDebugCamera_ = false;
#endif

}

void GameScene::Update() {
	//入力処理
	input_->Update();

	// プレイヤーの更新
	player_->Update(input_);

	//敵の更新
	enemy_->Update();

	//カメラ更新
	gameCamera_->Update();

	//首更新
	for (auto& neck : necks_) {
		neck->Update();
	}

	//ゴール更新処理
	goal_->Update();

	// コース
	course_->Update();

	// 当たり判定
	checkCollision_->Update();

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
}

void GameScene::Draw() {
	//描画処理
	player_->Draw();

	//敵の描画
	enemy_->Draw();

	//首描画
	for (auto& neck : necks_) {
		neck->Draw();
	}

	//ゴール描画処理
	goal_->Draw();

	// コース
	course_->Draw(directionalLight_);
}