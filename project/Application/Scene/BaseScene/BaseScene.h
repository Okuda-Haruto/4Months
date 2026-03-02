#pragma once
#include <Input/Input.h>

#include <memory>

//シーン
class BaseScene {
protected:
	//入力
	std::shared_ptr<Input> input_;
public:
	//初期化
	virtual void Initialize(std::shared_ptr<Input> input) = 0;
	//終了処理
	virtual void Finalize() = 0;
	//更新処理
	virtual void Update() = 0;
	//描画処理
	virtual void Draw() = 0;
};