#include "TitleScene.h"
#include "../../SceneManager/SceneManager.h"

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
	defaultCamera_->Update(SRT{ {1,1,1}, {}, {0,0, -50 } });

	bgm_ = make_unique<Audio>();
	bgm_->Initialize("resources/DebugResources/mokugyo.wav", 0.5f);
	selectSE_ = make_unique<Audio>();
	selectSE_->Initialize("resources/DebugResources/TestAudio_koukaonLabo.mp3", 0.5f);
}

void TitleScene::Finalize() {

}

void TitleScene::Update() {
	Keyboard keyboard = input_->GetKeyBoard();
	Pad pad = input_->GetPad(0);

	title_Sprite_->Update();
	logo_->Update();

	if (keyboard.trigger[DIK_SPACE] || pad.Button[PAD_BUTTON_B].trigger) {
		selectSE_->SoundPlayWave();
		SceneManager::GetInstance()->ChangeScene("Game");
	}

	//カメラアップデート
	if(isUseDebugCamera_){
		defaultCamera_->Update();
	} else {
		defaultCamera_->Update(SRT{ {1,1,1}, {}, {0,0, -80 } });
	}

	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	if (!bgm_->IsSoundPlayingWave()) {
		bgm_->SoundPlayWave();
	}

#ifdef USE_IMGUI
	if (ImGui::Button("デバッグカメラ")) {
		isUseDebugCamera_ = !isUseDebugCamera_;
	}

	// リセット
	if (keyboard.trigger[DIK_R]) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}
#endif
}

void TitleScene::Draw() {
	title_Sprite_->Draw2D();

	logo_->Draw();

}