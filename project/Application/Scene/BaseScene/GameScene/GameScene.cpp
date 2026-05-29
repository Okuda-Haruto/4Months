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

	course_ = std::make_unique<Course>();
	LoadCourse("resources/CourseData/Stage1.csv");

	// カメラ
	gameCamera_ = make_unique<GameCamera>();
	gameCamera_->Initialize(defaultCamera_, std::make_unique<StartCamera>(), input_, player_.get(), course_.get());

	// コース
	course_->Initialize(courseData_, gameCamera_.get(), directionalLight_);
	chunkHeight_ = int(course_->GetChunkData().size.y);
	float cameraPosBottom = -32 * float(course_->GetVoxel()->GetChunks().size() + 1) * 3.0f + 16.0f * 3.0f;
	gameCamera_->SetCameraPosBottom(cameraPosBottom);

	// 当たり判定
	checkCollision_ = std::make_unique<CheckCollision>();
	checkCollision_->Initialize(course_.get(), gameCamera_.get());

	// HUD
	hud_ = std::make_unique<HUD>();
	hud_->Initialize(input.get(), gameCamera_->GetCamera(), directionalLight_);

	// 予測表示
	hitPreview_ = std::make_unique<HitPreview>();
	hitPreview_->Initialize(directionalLight_);

	// 開始カウントダウン
	startCountdown_ = std::make_unique<StartCountdown>();
	startCountdown_->Initialize(directionalLight_);

	fade_ = std::make_unique<Fade>();
	fade_->Initialzie();
	fade_->SetFadeMode(Fade::FADE_MODE::FADE_IN);

	menu_ = std::make_unique<Menu>();
	menu_->Initialize(input_, fade_.get());


	// タイトルから遷移
	gameTransition = std::make_unique<Combine>();
	gameTransition->InitializeGame(directionalLight_, gameCamera_->GetCamera());
	gameTransition->Update();

	gameCamera_->Update();

#ifdef USE_IMGUI
	isUseDebugCamera_ = false;
#endif

	// bgm
	bgm_ = make_unique<Audio>();
	bgm_->Initialize("resources/SE・BGM/Game/bgm_game.mp3", 0.5f);
}

void GameScene::Finalize() {}

void GameScene::Update() {
	Keyboard keyboard = input_->GetKeyBoard();
	Pad pad = input_->GetPad(0);

	//メニュー中は一切動かさない
	if (menu_->GetPhase() != Menu::Menu_Phase::Idle && menu_->GetPhase() != Menu::Menu_Phase::End) {

		if (!startCountdown_->IsEnd()) {
			if (!gameTransition->IsEnd()) {
				gameTransition->Update();
			}
			else {

				// 開始カウントダウン
				startCountdown_->Update();

				player_->UpdateEffectsOnly();
				course_->Update(player_.get());

				if (startCountdown_->IsFirsttime()) {
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
					}
					else {
						skipHold_ = 0;
					}

				}
				else if (course_->GetResultState() == ResultState::End) {
					// 区間リザルト終了
					startCountdown_->SetResultEnd();

					if (startCountdown_->IsEnd()) {
						isSectionResult_ = false;
						gameCamera_->ChangeCamera(std::make_unique<DownCamera>(), 0.3f);
					}
				}
			}

		}
		else {

			// ★追加：簡易リザルト状態を渡す
			if (!isSectionResult_ && course_->InSubSection() && course_->GetResultState() <= ResultState::Wait) {
				isSectionResult_ = true;
				gameCamera_->ChangeCamera(std::make_unique<SectionResultCamera>(), 0.0f);
			}
			if (player_->IsBreak()) {
				gameCamera_->ChangeCamera(std::make_unique<SectionResultCamera>(), 0.0f);
			}

			player_->SetResult(isSectionResult_);
			player_->SetCanSkipResult(course_->GetResultState() == ResultState::Wait);

			player_->Update(input_, course_.get(), startCountdown_.get());

			if (isClear_) {
				if (isUp_) {
					clearY_ += GameEngine::GetDeltaTimeRate();
					if (clearY_ > 0) isUp_ = false;
				}
				else {
					clearY_ -= GameEngine::GetDeltaTimeRate();
					if (clearY_ < -32 * 3.0f * 4) isUp_ = true;
				}

				if (keyboard.trigger[DIK_SPACE] || pad.Button[PAD_BUTTON_B].trigger) {
					fade_->SetFadeMode(Fade::FADE_MODE::FADE_OUT);
				}
			}

			course_->Update(player_.get());

			checkCollision_->Update(player_.get());
			checkCollision_->UpdateImGui();

			hitPreview_->Update(player_.get(), checkCollision_.get());

			if (player_->IsEndResult()) {
				startCountdown_->Reset(player_->GetTransform().translate);
			}

			hud_->Update(player_.get(), course_.get(), course_->GetCurrentSection()->GetTimer(), int(0), gameCamera_->GetCamera());
		}

		//カメラ更新
		if (gameTransition->IsEnd()) {
			gameCamera_->Update();
		}

		if (isUseDebugCamera_) {
			defaultCamera_->Update();
		}
		directionalLight_->SetDirectionalLightElement(directionalLightElement_);

		hud_->SetPauseDisplay(!startCountdown_->IsEnd());

	}

	//メニュー
	menu_->Update();

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
	if (course_->InSubSection()) {
		ImGui::Text("破壊率 : %.1f%%", course_->GetPrevBreakRate());
	} else {
		ImGui::Text("破壊率 : %.1f%%", course_->GetCurrBreakRate());
	}
	ImGui::End();
#endif

	if (keyboard.trigger[DIK_R]) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}

	//ゲームオーバー画面のままおわらせない
	if (!player_->IsBreak() && !course_->GetIsSectionFailed()) {
		if (course_->isAllCleared()) {
			if (!isClear_) {
				gameCamera_->ChangeCamera(std::make_unique<ResultCamera>(), 1.0f);
			}
			isClear_ = true;
		} else if (course_->isEnd()) {
			fade_->SetFadeMode(Fade::FADE_MODE::FADE_OUT);
		}
	}

	if (fade_->GetIsEnd() && fade_->GetFadeMode() == Fade::FADE_MODE::FADE_OUT) {
		if (menu_->GetPhase() == Menu::Menu_Phase::End) {
			switch (menu_->GetState())
			{
			case Menu::Menu_State::Retry:
				SceneManager::GetInstance()->ChangeScene("Game");
				break;
			case Menu::Menu_State::Title:
				SceneManager::GetInstance()->ChangeScene("Title");
				break;
			case Menu::Menu_State::Select:
				SceneManager::GetInstance()->SetIsSelect(true);
				SceneManager::GetInstance()->ChangeScene("Title");
				break;
			default:
				break;
			}
		}
		else {
			SceneManager::GetInstance()->ChangeScene("Title");
		}
	}

	fade_->Update();

	GameEngine::RenderPreDraw("BackGround");

	skydome_->Draw3DNoFog();
	course_->DrawGoalBarrier();

	GameEngine::RenderPostDraw("BackGround");

	GameEngine::RenderPreDraw("Play");

	skydome_->Draw3DNoFog();

	if (!player_->IsBreak()) {
		if (isClear_) {
			course_->DrawAll(directionalLight_);
		}
		else if (startCountdown_->IsPreStart() && startCountdown_->IsFirsttime()) {
			course_->DrawUp(directionalLight_);
		}
		else {

			course_->Draw(directionalLight_);
			hitPreview_->Draw();
			startCountdown_->Draw();
			player_->Draw();
		}
	}
	else {
		course_->DrawGameOver();
	}

	if (!gameTransition->IsEnd()) {
		gameTransition->Draw();
	}

	GameEngine::RenderPostDraw("Play");

	GameEngine::RenderPreDraw("Screen");

	GameEngine::DrawScreen(TextureManager::GetInstance()->GetSrvIndex("Play"));

	hud_->Draw();

	startCountdown_->Draw();

	GameEngine::RenderPostDraw("Screen");
}

void GameScene::Draw() {

	GameEngine::DrawScreen(TextureManager::GetInstance()->GetSrvIndex("Screen"));


	menu_->Draw();

	fade_->Draw();
}

void GameScene::LoadCourse(std::string filePath) {
	std::ifstream file(filePath);
	assert(file.is_open());

	// 1行分の文字列を入れる変数
	std::string line;

	while (std::getline(file, line)) {

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		std::getline(line_stream, word, ',');

		// コメント
		if (word.find("//") == 0) {
			continue;
		}

		// カンマ区切りで読む
		if (word.find("ChunkSize") == 0) {
			std::getline(line_stream, word, ',');
			courseData_.csvData.size.x = stof(word);
			std::getline(line_stream, word, ',');
			courseData_.csvData.size.y = stof(word);
			std::getline(line_stream, word, ',');
			courseData_.csvData.size.z = stof(word);
		}

		if (word.find("ChunkDataDirectoryPath") == 0) {
			std::getline(line_stream, word, ',');
			courseData_.csvData.chunkDataDirectoryPath = word;
		}

		if (word.find("VoxelDataFilePath") == 0) {
			std::getline(line_stream, word, ',');
			courseData_.csvData.voxelDataFilePath = word;
		}

		if (word.find("Section") == 0) {
			SectionData section;
			std::getline(line_stream, word, ',');
			section.startChunkY = stoi(word);
			std::getline(line_stream, word, ',');
			section.endChunkY = stoi(word);
			std::getline(line_stream, word, ',');
			section.maxSeconds = stof(word);
			std::getline(line_stream, word, ',');
			section.clearScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.maxScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.rankBorders.rate.aScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.rankBorders.rate.bScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.rankBorders.count.aScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.rankBorders.count.bScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.rankBorders.time.aScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.rankBorders.time.bScore = stoi(word);

			courseData_.sectionDatas.push_back(section);
		}
	}

	file.close();
}