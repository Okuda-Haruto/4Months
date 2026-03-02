#include "TitleScene.h"
#include "../../SceneManager/SceneManager.h"

void TitleScene::Initialize(std::shared_ptr<Input> input) {
	input_ = input;

	title_Sprite_ = std::make_unique<Sprite>();

	title_Sprite_->Initialize("resources/HUD/Title/Title.png");
	title_Sprite_->SetPosition(Vector2{ 0,0 });
}

void TitleScene::Finalize() {

}

void TitleScene::Update() {
	Keyboard keyboard = input_->GetKeyBoard();
	Pad pad = input_->GetPad(0);

	title_Sprite_->Update();

	if (keyboard.trigger[DIK_SPACE]) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
}

void TitleScene::Draw() {
	title_Sprite_->Draw2D();
}