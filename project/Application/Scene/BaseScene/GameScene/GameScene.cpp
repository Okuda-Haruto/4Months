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

	skydome_ = std::make_unique<Object>();
	skydome_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/GoalBarrier", "Skydome.obj"));
	skydome_->SetReflection(REFLECTION_None);
	skydome_->SetCamera(defaultCamera_);

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(Vector3{ 0,200,0 }, directionalLight_, defaultCamera_);

	CSVData courseData;
	courseData.size = { 2,18,2 };
	courseData.directoryPath = "resources/CSV";

	course_ = std::make_unique<Course>();

	// カメラ
	gameCamera_ = make_unique<GameCamera>();
	gameCamera_->Initialize(defaultCamera_, std::make_unique<StartCamera>(), input_, player_.get(), course_.get());

	// コース
	course_->Initialize(courseData, gameCamera_.get(), directionalLight_);
	chunkHeight_ = int(course_->GetChunkData().size.y);
	float cameraPosBottom = -32 * float(course_->GetVoxel()->GetChunks().size() + 1) * 3.0f + 16.0f * 3.0f;
	gameCamera_->SetCameraPosBottom(cameraPosBottom);

	// 当たり判定
	checkCollision_ = std::make_unique<CheckCollision>();
	checkCollision_->Initialize(course_.get(), gameCamera_.get());

	// HUD
	hud_ = std::make_unique<HUD>();
	hud_->Initialize(input.get(), gameCamera_->GetCamera());

	// 予測表示
	hitPreview_ = std::make_unique<HitPreview>();
	hitPreview_->Initialize(directionalLight_);

	// 開始カウントダウン
	startCountdown_ = std::make_unique<StartCountdown>();
	startCountdown_->Initialize(directionalLight_);

	// タイトルから遷移
	combine_ = std::make_unique<Combine>();
	combine_->InitializeGame(directionalLight_);

#ifdef USE_IMGUI
	isUseDebugCamera_ = false;
#endif

	// bgm
	bgm_ = make_unique<Audio>();
	bgm_->Initialize("resources/SE・BGM/Game/bgm_game.mp3", 0.5f);
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	Keyboard keyboard = input_->GetKeyBoard();
	Pad pad = input_->GetPad(0);

	if (!startCountdown_->IsEnd()) {
		if (!combine_->IsEnd()) {
			combine_->Update();
		} else {

			// 開始カウントダウン
			startCountdown_->Update();

			// ゲーム中のカメラに移行
			if (startCountdown_->IsDownCameraTime()) {
				gameCamera_->ChangeCamera(std::make_unique<DownCamera>(), 2.0f);
			}

			if ((keyboard.hold[DIK_SPACE] || pad.Button[PAD_BUTTON_B].hold) && startCountdown_->IsPreStart()) {
				skipHold_ += GameEngine::GetDeltaTime();
				if (skipHold_ > 0.5f) {
					gameCamera_->ChangeCamera(std::make_unique<DownCamera>(), 0.0f);
					startCountdown_->SkipPreStart();
				}

#ifdef USE_IMGUI
				gameCamera_->ChangeCamera(std::make_unique<DownCamera>(), 0.0f);
				startCountdown_->SkipAll();
#endif
			} else {
				skipHold_ = 0;
			}
		}

	} else {

		// プレイヤーの更新
		player_->Update(input_);

		if (isClear_) {
			if (isUp_) {
				clearY_ += GameEngine::GetDeltaTimeRate();
				if (clearY_ > 0) {
					isUp_ = false;
				}
			} else {
				clearY_ -= GameEngine::GetDeltaTimeRate();;
				if (clearY_ < -32 * 3.0f * 4) {
					isUp_ = true;
				}
			}

			//クリアしてるならタイトルに戻れる
			if (keyboard.trigger[DIK_SPACE] || pad.Button[PAD_BUTTON_B].trigger) {
				SceneManager::GetInstance()->ChangeScene("Title");
			}

		}

		// コース
		course_->Update(player_.get());

		// 当たり判定
		checkCollision_->Update(player_.get());
		checkCollision_->UpdateImGui();

		// 予測表示
		hitPreview_->Update(player_.get(), checkCollision_.get());
	}

	//カメラ更新
	if (combine_->IsEnd()) {
		gameCamera_->Update();
	}

	//カメラアップデート
	if (isUseDebugCamera_) {
		defaultCamera_->Update();
	}
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	hud_->SetPauseDisplay(!startCountdown_->IsEnd());

	// HUD
	hud_->Update(player_.get(), course_.get(), course_->GetCurrentSection()->GetTimer(), int(0), gameCamera_->GetCamera());

	if (!bgm_->IsSoundPlayingWave()) {
		bgm_->SoundPlayWave();
	}

#ifdef USE_IMGUI
	int section = course_->GetCurrentSectionNumber();

	ImGui::Begin("GameScene");
	if (ImGui::Button("デバッグカメラ")) {
		isUseDebugCamera_ = !isUseDebugCamera_;
	}
	ImGui::Text("現在区間 : %d", section);
	ImGui::End();
#endif

	//仮置き
	if (keyboard.trigger[DIK_R]) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}

	if (course_->isAllCleared()) {
		if (!isClear_) {
			// クリア
			clearCameraTransform_.translate = { 0,-16 * 3 * 2,-16 * 3 - 300 };
			clearCameraTransform_.rotate = IdentityQuaternion();
			clearCameraTransform_.scale = { 1,1,1 };
			gameCamera_->ChangeCamera(std::make_unique<ResultCamera>(), 1.0f);
		}
		isClear_ = true;
	} else if (course_->isEnd()) {
		// 失敗
		SceneManager::GetInstance()->ChangeScene("Title");
	}

	GameEngine::RenderPreDraw("BackGround", 0);

	course_->DrawGoalBarrier();

	GameEngine::RenderPostDraw("BackGround");
}

void GameScene::Draw() {

	//背景描画
	//GameEngine::DrawScreen(TextureManager::GetInstance()->GetSrvIndex("BackGround"));

	skydome_->Draw3DNoFog();

	// コース
	if (isClear_) {
		course_->DrawAll(directionalLight_);
	} else if (startCountdown_->IsPreStart()) {
		course_->DrawUp(directionalLight_);
	} else {

		if (course_->InSubSection()) {
			// 区間記録表示中
			course_->DrawAll(directionalLight_);
		} else {
			course_->Draw(directionalLight_);
		}
		player_->Draw();

		// HUD
		hud_->Draw();

		hitPreview_->Draw();

		// 開始カウントダウン
		startCountdown_->Draw();
	}

	if (!combine_->IsEnd()) {
		combine_->Draw();
	}
}