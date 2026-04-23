#pragma once
#include <GameEngine.h>

class Fade {
public:

	enum class FADE_MODE {
		FADE_IN,
		FADE_OUT
	};
private:

	std::unique_ptr<Sprite> sprite_;

	const float kMaxFadeTime_ = 0.5f;
	float fadeTime_;

	FADE_MODE fadeMode_;

	bool isEnd_;
	bool isFirstFlame_;

public:

	void Initialzie();

	void Update();

	void Draw();

	void SetFadeMode(FADE_MODE fadeMode);


	bool GetIsEnd() { return isEnd_; }
	FADE_MODE GetFadeMode() { return fadeMode_; }
};