#include "GameEngine.h"
#include "SampleScene/SampleScene.h"
#include "Scene/SceneFactory/SceneFactory.h"
#include "Scene/SceneManager/SceneManager.h"

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//ゲームエンジン
	GameEngine::Initialize(L"LE2A_03_オクダ_ハルト", 1280, 720);

	//シーンマネージャー生成
	unique_ptr<SceneFactory> sceneFactory = make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory.get());
	shared_ptr<Input> input = make_shared<Input>();
	input->Initialize(GameEngine::GetWindowsAPI());
	SceneManager::GetInstance()->SetInput(input);

	SceneManager::GetInstance()->ChangeScene("Game");

	//ウィンドウの×ボタンが押されるまでループ
	while (GameEngine::WindowState()) {
		if (GameEngine::StartFlame()) {

			//入力処理
			input->Update();

			//
			//	更新処理
			//

			SceneManager::GetInstance()->Update();

			//
			//	描画処理
			//

			GameEngine::PreDraw();

			SceneManager::GetInstance()->Draw();

			GameEngine::PostDraw();

		}
	}

	input.reset();
	sceneFactory.reset();
	SceneManager::GetInstance()->Finalize();
	GameEngine::Finalize();
	
	return 0;
}