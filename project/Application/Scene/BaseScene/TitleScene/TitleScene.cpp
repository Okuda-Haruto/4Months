#include "TitleScene.h"
#include "../../SceneManager/SceneManager.h"
#include <numbers>

void TitleScene::Initialize(std::shared_ptr<Input> input) {
	input_ = input;

	title_Sprite_ = std::make_unique<Sprite>();

	title_Sprite_->Initialize("resources/HUD/Title/Title.png");
	title_Sprite_->SetPosition(Vector2{ 0,0 });

	//光源
	directionalLight_ = make_shared<DirectionalLight>();
	directionalLight_->Initialize(GameEngine::GetDirectXCommon());
	directionalLightElement_ = {
		{1.0f,1.0f,1.0f,1.0f},
		Normalize(Vector3{0.0f,-1.0f,0.25f}),
		1.0f
	};
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	logo_ = std::make_unique<TitleMoji>();
	logo_->Initialize(directionalLight_, input_);

	//デバッグカメラ
	debugCamera_ = make_shared<DebugCamera>();
	debugCamera_->Initialize(input_);

	//カメラ
	defaultCamera_ = Object::GetDefaultCamera();
	defaultCamera_->SetDebugCamera(debugCamera_);
	defaultCamera_->Update(SRT{ {1,1,1}, {}, {0,0, -75 } });

	//カメラ
	studioCamera_ = std::make_shared<Camera>();
	studioCamera_->Initialize(GameEngine::GetDirectXCommon(), 500, 0, 2000, 2000);
	studioGameCamera_ = std::make_shared<GameCamera>();
	studioGameCamera_->Initialize(studioCamera_, std::make_unique<StudioCamera>(), input, nullptr, nullptr, nullptr);

	livingRoomCamera_ = std::make_shared<Camera>();
	livingRoomCamera_->Initialize(GameEngine::GetDirectXCommon(), 500, 0, 2000, 2000);
	livingGameCamera_ = std::make_shared<GameCamera>();
	livingGameCamera_->Initialize(livingRoomCamera_, std::make_unique<LivingCamera>(), input, nullptr, nullptr, nullptr);

	studio_ = std::make_unique<Object>();
	studio_->Initialize(ModelManager::GetInstance()->GetModel("resources/Title", "TV_Stadio.obj"));
	studio_->SetDirectionalLight(directionalLight_);
	studio_->SetCamera(studioCamera_);

	livingRoom_ = std::make_unique<Object>();
	livingRoom_->Initialize(ModelManager::GetInstance()->GetModel("resources/Title", "room.obj"));
	livingRoom_->SetDirectionalLight(directionalLight_);
	livingRoom_->SetCamera(livingRoomCamera_);

	for (int i = 0; i < humans_.size(); i++) {
		humans_[i] = std::make_unique<Object>();
		humans_[i]->Initialize(ModelManager::GetInstance()->GetModel("resources/Player", "moderator_master.gltf"));
		humans_[i]->SetCamera(studioCamera_);
		humans_[i]->SetDirectionalLight(directionalLight_);
		human_Tranaforms_[i].scale = { 3,3,3 };
		humans_[i]->SetTransform(human_Tranaforms_[i]);
		humans_[i]->SetAnimationIndex(0);
	}

	human_Tranaforms_[0].translate = { 55,25,-28 };
	human_Tranaforms_[0].rotate = MakeRotateAxisAngleQuaternion({ 0,1,0 }, std::numbers::pi_v<float> / 180 * 145);
	humans_[0]->SetShininess(60);
	humans_[0]->SetAnimationIndex(0);
	humans_[0]->SetIsUseAnimation(true);
	humans_[0]->SetIsLoopAnimation(true);

	for (int i = 0; i < humans_.size(); i++) {
		humans_[i]->SetTransform(human_Tranaforms_[i]);
	}

	titleScreen_ = std::make_unique<TitleScreen>();
	titleScreen_->Initialize(livingGameCamera_);

	static int TVindex = 19;

	std::vector<Parts> parts = livingRoom_->GetParts();
	parts[TVindex].textureIndex = TextureManager::GetInstance()->GetSrvIndex("BackGround");
	parts[TVindex].material->reflection = REFLECTION_None;
	livingRoom_->SetParts(parts[TVindex], TVindex);

	fade_ = std::make_unique<Fade>();
	fade_->Initialzie();
	fade_->SetFadeMode(Fade::FADE_MODE::FADE_IN);

	gameTransition = std::make_unique<Combine>();
	gameTransition->InitializeTitle(directionalLight_, defaultCamera_);

	bgm_ = make_unique<Audio>();
	bgm_->Initialize("resources/SE・BGM/Title/bgm_title.mp3", 0.5f);
	selectSE_ = make_unique<Audio>();
	selectSE_->Initialize("resources/SE・BGM/deceid.mp3", 0.5f);

	if (SceneManager::GetInstance()->GetIsSelect()) {
		studioGameCamera_->ChangeCamera(std::make_unique<SelectCamera>(), 0.0f);
		livingGameCamera_->ChangeCamera(std::make_unique<TVCamera>(), 0.0f);
		isChangeStudioCamera_ = true;
		logo_->SetEnd();
		titleScreen_->SetEnd();

		SceneManager::GetInstance()->SetIsSelect(false);
	}

	selectNum_ = 1;
}

void TitleScene::Finalize() {

}

void TitleScene::Update() {
	Keyboard keyboard = input_->GetKeyBoard();
	Pad pad = input_->GetPad(0);

	title_Sprite_->Update();

	if (!titleScreen_->IsEnd()) {
		titleScreen_->Update();
	}
	if (titleScreen_->IsEnd() && !logo_->IsEnd()) {
		logo_->Update();
	}

	//文字が爆散したらセレクトに行く
	if (logo_->IsEnd() && !isChangeStudioCamera_) {
		studioGameCamera_->ChangeCamera(std::make_unique<SelectCamera>(), 2.0f);
		isChangeStudioCamera_ = true;
	}

	//全て終わった後にSpace(Bボタン)でステージに
	if ((studioGameCamera_->IsEndChangeCamera() && isChangeStudioCamera_ && fade_->GetIsEnd() && fade_->GetFadeMode() == Fade::FADE_MODE::FADE_IN)) {
		if ((keyboard.trigger[DIK_SPACE] || pad.Button[PAD_BUTTON_B].trigger)) {
			SceneManager::GetInstance()->SetSelectNum(selectNum_);

			selectSE_->SoundPlayWave();
			sceneChange_ = true;
		}
		else if ((keyboard.trigger[DIK_D] || keyboard.trigger[DIK_RIGHT] || pad.Button[PAD_BUTTON_RIGHT].trigger)) {
			selectNum_++;
			if (selectNum_ >= 4) {
				selectNum_ = 1;
			}
			auto parts = studio_->GetParts();
			switch (selectNum_)
			{
			case 1:
				parts[5].textureIndex = TextureManager::GetInstance()->GetSrvIndex("resources/Title/texture_stadio/Stage1.png");
				break;
			case 2:
				parts[5].textureIndex = TextureManager::GetInstance()->GetSrvIndex("resources/Title/texture_stadio/Stage2.png");
				break;
			case 3:
				parts[5].textureIndex = TextureManager::GetInstance()->GetSrvIndex("resources/Title/texture_stadio/Stage3.png");
				break;
			default:
				break;
			}
			studio_->SetParts(parts[5], 5);
		}
		else if ((keyboard.trigger[DIK_A] || keyboard.trigger[DIK_LEFT] || pad.Button[PAD_BUTTON_LEFT].trigger)) {
			selectNum_--;
			if (selectNum_ <= 0) {
				selectNum_ = 3;
			}
			auto parts = studio_->GetParts();
			switch (selectNum_)
			{
			case 1:
				parts[5].textureIndex = TextureManager::GetInstance()->GetSrvIndex("resources/Title/texture_stadio/Stage1.png");
				break;
			case 2:
				parts[5].textureIndex = TextureManager::GetInstance()->GetSrvIndex("resources/Title/texture_stadio/Stage2.png");
				break;
			case 3:
				parts[5].textureIndex = TextureManager::GetInstance()->GetSrvIndex("resources/Title/texture_stadio/Stage3.png");
				break;
			default:
				break;
			}
			studio_->SetParts(parts[5], 5);
		}
	}
	//スキップ
	else if ((keyboard.trigger[DIK_SPACE] || pad.Button[PAD_BUTTON_B].trigger) && isChangeStudioCamera_ && !isBackTitle_) {
		studioGameCamera_->ChangeCamera(std::make_unique<SelectCamera>(), 0.0f);
	}

	//全て終わった後にESCAPE(ボタン)でステージに
	if ((keyboard.trigger[DIK_ESCAPE] || pad.Button[PAD_BUTTON_START].trigger) && (studioGameCamera_->IsEndChangeCamera() && isChangeStudioCamera_ && fade_->GetIsEnd() && fade_->GetFadeMode() == Fade::FADE_MODE::FADE_IN)) {
		selectSE_->SoundPlayWave();
		isBackTitle_ = true;
		studioGameCamera_->ChangeCamera(std::make_unique<StudioCamera>(), 2.0f);
	}

	if (sceneChange_) {
		gameTransition->Update();
		if (gameTransition->IsEnd()) {
			SceneManager::GetInstance()->ChangeScene("Game");
		}
	}

	if (isBackTitle_) {
		if (studioGameCamera_->IsEndChangeCamera()) {
			logo_.reset();
			logo_ = std::make_unique<TitleMoji>();
			logo_->Initialize(directionalLight_, input_);
			isBackTitle_ = false;
			isChangeStudioCamera_ = false;
		}
	}

	if (fade_->GetIsEnd() && fade_->GetFadeMode() == Fade::FADE_MODE::FADE_OUT) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}

	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	if (!bgm_->IsSoundPlayingWave()) {
		bgm_->SoundPlayWave();
	}

#ifdef USE_IMGUI
	// リセット
	if (keyboard.trigger[DIK_R]) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}

	std::string str;
	for (int i = 0; i < humans_.size(); i++) {
		str = "human" + std::to_string(i) + " transate";
		ImGui::DragFloat3(str.c_str(), &human_Tranaforms_[i].translate.x,0.01f);
		str = "human" + std::to_string(i) + " scale";
		ImGui::DragFloat3(str.c_str(), &human_Tranaforms_[i].scale.x, 0.01f);
		humans_[i]->SetTransform(human_Tranaforms_[i]);
	}

#endif

	for (int i = 0; i < humans_.size(); i++) {
		humans_[i]->Update();
	}

	studioGameCamera_->Update();
	livingGameCamera_->Update();

	studio_->Update();
	livingRoom_->Update();

	fade_->Update();

	GameEngine::RenderPreDraw("BackGround");

	if (!gameTransition->IsPlaying()) {
		studio_->Draw3DNoFog();

		for (int i = 0; i < 1; i++) {
			humans_[i]->Draw3DNoFog();
		}

		//title_Sprite_->Draw2D();

		logo_->Draw();
	}
	gameTransition->Draw();

	titleScreen_->Draw();

	GameEngine::RenderPostDraw("BackGround");
}

void TitleScene::Draw() {
	static int TVindex = 19;

	std::vector<Parts> parts = livingRoom_->GetParts();

	for (int i = 0; i < parts.size(); i++) {
		if (i == TVindex) continue;
		livingRoom_->Draw3DNoFog(i);
	}
	livingRoom_->Draw3DNoFogRender(TVindex);

	fade_->Draw();


	//GameEngine::DrawScreen(TextureManager::GetInstance()->GetSrvIndex("BackGround"));
}