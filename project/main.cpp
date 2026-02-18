#include "GameEngine.h"
#include "SampleScene/SampleScene.h"
#include "GameScene/GameScene.h"

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//ゲームエンジン
	GameEngine::Initialize(L"LE2A_03_オクダ_ハルト", 1280, 720);

	//unique_ptr<SampleScene> gameManager = make_unique<SampleScene>();
	//gameManager->Initialize();
	unique_ptr<GameScene> gameScene = make_unique<GameScene>();
	gameScene->Initialize();

	//ウィンドウの×ボタンが押されるまでループ
	while (GameEngine::WindowState()) {
		if (GameEngine::StartFlame()) {

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

		}
	}

	gameScene.reset();
	GameEngine::Finalize();
	
	return 0;
}