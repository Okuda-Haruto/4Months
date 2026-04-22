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
	logo_->Initialize(directionalLight_);

	//デバッグカメラ
	debugCamera_ = make_shared<DebugCamera>();
	debugCamera_->Initialize(input_);

	//カメラ
	defaultCamera_ = Object::GetDefaultCamera();
	defaultCamera_->SetDebugCamera(debugCamera_);
	defaultCamera_->Update(SRT{ {1,1,1}, {}, {0,0, -75 } });

	cameraTransform_ = SRT{ {1,1,1},
		{MakeRotateAxisAngleQuaternion(Vector3{0,1,0},std::numbers::pi_v<float>)},
		{0,35, 120}
	};

	//カメラ
	studioCamera_ = std::make_shared<Camera>();
	studioCamera_->Initialize(GameEngine::GetDirectXCommon(), 500, 0, 2000, 2000);
	studioCamera_->SetDebugCamera(debugCamera_);
	studioCamera_->Update(cameraTransform_);

	livingRoomCamera_ = std::make_shared<Camera>();
	livingRoomCamera_->Initialize(GameEngine::GetDirectXCommon(),500,0,2000,2000);
	gameCamera_ = std::make_shared<GameCamera>();
	gameCamera_->Initialize(livingRoomCamera_, std::make_unique<LivingCamera>(), input, nullptr, nullptr);

	studio_ = std::make_unique<Object>();
	studio_->Initialize(ModelManager::GetInstance()->GetModel("resources/Title", "Stadio.obj"));
	studio_->SetDirectionalLight(directionalLight_);
	studio_->SetCamera(studioCamera_);

	livingRoom_ = std::make_unique<Object>();
	livingRoom_->Initialize(ModelManager::GetInstance()->GetModel("resources/Title", "LivingRoom.obj"));
	livingRoom_->SetDirectionalLight(directionalLight_);
	livingRoom_->SetCamera(livingRoomCamera_);

	titleScreen_ = std::make_unique<TitleScreen>();
	titleScreen_->Initialize(gameCamera_);

	static int TVindex;

	std::vector<Parts> parts = livingRoom_->GetParts();
	parts[1].textureIndex = TextureManager::GetInstance()->GetSrvIndex("BackGround");
	parts[1].material->reflection = REFLECTION_None;
	livingRoom_->SetParts(parts[1], 1);

	combine_ = std::make_unique<Combine>();
	combine_->InitializeTitle(directionalLight_);

	bgm_ = make_unique<Audio>();
	bgm_->Initialize("resources/SE・BGM/Title/bgm_title.mp3", 0.5f);
	selectSE_ = make_unique<Audio>();
	selectSE_->Initialize("resources/SE・BGM/deceid.mp3", 0.5f);
}

void TitleScene::Finalize() {

}

void TitleScene::Update() {
	Keyboard keyboard = input_->GetKeyBoard();
	Pad pad = input_->GetPad(0);

	title_Sprite_->Update();
	titleScreen_->Update();
	if (titleScreen_->IsEnd()) {
		logo_->Update();

		if (logo_->IsEnd()) {
			combine_->Update();

			if (combine_->IsEnd()) {
				SceneManager::GetInstance()->ChangeScene("Game");
			}
		}
	}

	if (keyboard.trigger[DIK_SPACE] || pad.Button[PAD_BUTTON_B].trigger) {
		selectSE_->SoundPlayWave();
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

	ImGui::DragFloat3("studioCamera translate", &cameraTransform_.translate.x);

#endif

	studioCamera_->Update(cameraTransform_);
	gameCamera_->Update();

	studio_->Update();
	livingRoom_->Update();

	GameEngine::RenderPreDraw("BackGround", 0);

	studio_->Draw3DNoFog();

	title_Sprite_->Draw2D();

	logo_->Draw();
	combine_->Draw();

	titleScreen_->Draw();

	GameEngine::RenderPostDraw("BackGround");
}

void TitleScene::Draw() {

	livingRoom_->Draw3DNoFog(0);
	livingRoom_->Draw3DNoFogRender(1);

	//GameEngine::DrawScreen(TextureManager::GetInstance()->GetSrvIndex("BackGround"));
}