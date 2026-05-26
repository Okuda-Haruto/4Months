#include "Menu.h"
#include <GameEngine.h>
#include <ModelManager/ModelManager.h>

void Menu::Initialize(std::shared_ptr<Input> input, Fade* fade) {
	input_ = input;
	fade_ = fade;

	camera_ = std::make_shared<Camera>();
	camera_->Initialize(GameEngine::GetDirectXCommon(), 500, 0, 2000, 2000);

	SRT transform{};
	transform.scale = { 1,1,1 };
	transform.rotate = IdentityQuaternion();
	transform.translate = { 0.05f,-0.175f,-8.25f };

	menu_Base_ = std::make_unique<Object>();
	menu_Base_->Initialize(ModelManager::GetInstance()->GetModel("resources/Menu", "Menu_Base.gltf"));
	menu_Base_->SetIsUseAnimation(true);
	menu_Base_->SetAnimationIndex(1);
	menu_Base_->ResetAnimationTime();
	menu_Base_->SetAnimationTime(1.0f);
	menu_Base_->SetReflection(REFLECTION_None);
	menu_Base_->SetCamera(camera_);
	menu_Base_->SetTransform(transform);
	menu_Base_->Update();

	menu_ = std::make_unique<Object>();
	menu_->Initialize(ModelManager::GetInstance()->GetModel("resources/Menu", "Menu.obj"));
	menu_->SetReflection(REFLECTION_None);
	menu_->SetCamera(camera_);
	menu_->SetTransform(transform);

	std::vector<Parts> parts = menu_->GetParts();

	boneMatrix_ = std::make_shared<Matrix4x4>();
	*boneMatrix_ = menu_Base_->GetBones()[0].finalMatrix;

	for (int i = 0; i < parts.size();i++) {
		parts[i].parent = boneMatrix_;
		menu_->SetParts(parts[i], i);
	}

	phase_ = Menu_Phase::None;
	state_ = Menu_State::Retry;
	isPlessBotton_ = false;

}

void Menu::Update() {
	Keyboard key = input_->GetKeyBoard();
	Pad pad = input_->GetPad(0);

	if (selectTime_ > 0.0f) {
		selectTime_ -= GameEngine::GetDeltaTime();
		if (selectTime_ < 0.0f) {
			selectTime_ = 0.0f;
		}
	}

	menu_Base_->Update();

	//項目用ボーン行列
	*boneMatrix_ = menu_Base_->GetBones()[0].finalMatrix *
		MakeQuaternionMatrix(menu_Base_->GetTransform().scale,
			menu_Base_->GetTransform().rotate,
			menu_Base_->GetTransform().translate);

	std::vector<Parts> parts = menu_->GetParts();

	for (int i = 0; i < 3; i++) {
		if (i == state_) {
			parts[i + 2].material->color = { 1.0f,1.0f,0.5f,1.0f };
		}
		else {
			parts[i + 2].material->color = { 1.0f,1.0f,1.0f,1.0f };
		}
	}

	parts[0].UVtransform.translate.y += 0.05f * GameEngine::GetDeltaTime();
	menu_->SetParts(parts[0], 0);

	switch (phase_)
	{
	case Menu::None:
		if (key.trigger[DIK_ESCAPE] || pad.Button[PAD_BUTTON_START].trigger) {
			menu_Base_->SetIsLoopAnimation(false);
			menu_Base_->SetAnimationIndex(2);	//Idle
			menu_Base_->ResetAnimationTime();

			phase_ = Start;
		}
		break;
	case Menu::Start:
		if (menu_Base_->IsEndAnimation()) {
			menu_Base_->SetIsLoopAnimation(true);
			menu_Base_->SetAnimationIndex(0);	//Idle
			menu_Base_->ResetAnimationTime();

			phase_ = Idle;
		}
		break;
	case Menu::Idle:
		if (key.trigger[DIK_ESCAPE] || pad.Button[PAD_BUTTON_START].trigger) {
			menu_Base_->SetIsLoopAnimation(false);
			menu_Base_->SetAnimationIndex(1);	//Idle
			menu_Base_->ResetAnimationTime();

			phase_ = End;
		}
		else if (key.trigger[DIK_SPACE] || pad.Button[PAD_BUTTON_B].trigger) {
			fade_->SetFadeMode(Fade::FADE_MODE::FADE_OUT);

			menu_Base_->SetIsLoopAnimation(false);
			menu_Base_->SetAnimationIndex(1);	//Idle
			menu_Base_->ResetAnimationTime();

			phase_ = End;
			isPlessBotton_ = true;
		}
		else {

			if (key.trigger[DIK_W] || key.trigger[DIK_UP] || pad.Button[PAD_BUTTON_UP].trigger ||
				(pad.LeftStick.magnitude > 0.7f && pad.LeftStick.vector.y > 0.5f && selectTime_ <= 0.0f)) {

				state_--;
				if (state_ < Menu_State::Retry) {
					state_ = Menu_State::Select;
				}

				selectTime_ += kMaxSelectTime_;
			}
			else if (key.trigger[DIK_S] || key.trigger[DIK_DOWN] || pad.Button[PAD_BUTTON_DOWN].trigger ||
				(pad.LeftStick.magnitude > 0.7f && pad.LeftStick.vector.y < -0.5f && selectTime_ <= 0.0f)) {

				state_++;
				if (state_ > Menu_State::Select) {
					state_ = Menu_State::Retry;
				}

				selectTime_ += kMaxSelectTime_;
			}
		}
		break;
	case Menu::End:
		if (menu_Base_->IsEndAnimation() && !isPlessBotton_) {

			phase_ = None;
		}
		break;
	default:
		break;
	}
}

void Menu::Draw() {
	if (phase_ != None) {
		menu_Base_->Draw3DNoFog();
		menu_->Draw3DNoFog();
	}
}