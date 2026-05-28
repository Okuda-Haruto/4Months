#pragma once
#include <Object/Object.h>
#include <Input/Input.h>
#include <Fade/Fade.h>
#include <memory>

class Menu {
public:
	enum Menu_Phase {
		None,
		Start,
		Idle,
		End,
	};

	enum Menu_State {
		Retry,
		Title,
		Select
	};

private:
	std::unique_ptr<Object> menu_Base_;
	std::unique_ptr <Object> menu_;

	std::shared_ptr<Input> input_;
	std::shared_ptr<Camera> camera_;

	std::shared_ptr<Matrix4x4> boneMatrix_;

	Menu_Phase phase_;
	int8_t state_;

	Fade* fade_;

	const float kMaxSelectTime_ = 0.25f;
	float selectTime_;

	bool isPlessBotton_;

public:

	void Initialize(std::shared_ptr<Input> input, Fade* fade);

	void Update();

	void Draw();

	Menu_Phase GetPhase() { return phase_; }
	int8_t GetState() { return state_; }

	void Reset() { phase_ = Start; }
};