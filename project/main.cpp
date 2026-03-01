#include "GameEngine.h"
#include "SampleScene/SampleScene.h"
#include "GameScene/GameScene.h"

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//ゲームエンジン
	GameEngine::Initialize(L"LE2A_03_オクダ_ハルト", 1280, 720);

	//一旦
	shared_ptr<Input> input = make_shared<Input>();
	input->Initialize(GameEngine::GetWindowsAPI());

	//unique_ptr<SampleScene> gameManager = make_unique<SampleScene>();
	//gameManager->Initialize();
	unique_ptr<GameScene> gameScene = make_unique<GameScene>();
	gameScene->Initialize(input);

	//ウィンドウの×ボタンが押されるまでループ
	while (GameEngine::WindowState()) {
		if (GameEngine::StartFlame()) {

			//入力処理
			input->Update();

			//
			//	更新処理
			//

			//gameManager->Update();
			gameScene->Update();

			//
			//	描画処理
			//

			GameEngine::PreDraw();

			//gameManager->Draw();
			gameScene->Draw();

			GameEngine::PostDraw();


			if (input->PushKey(DIK_R)) {
				gameScene.reset();
				gameScene = make_unique<GameScene>();
				gameScene->Initialize(input);
			}
		}
	}

	gameScene.reset();
	GameEngine::Finalize();
	
	return 0;
}