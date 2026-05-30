#include "GameEngine.h"
#include "SampleScene/SampleScene.h"
#include "Scene/SceneFactory/SceneFactory.h"
#include "Scene/SceneManager/SceneManager.h"

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//ゲームエンジン
	GameEngine::Initialize(L"3108_災Guy", 1280, 720);

	TextureManager::GetInstance()->MakeRenderTexture("Sky");
	TextureManager::GetInstance()->MakeRenderTexture("BackGround");
	TextureManager::GetInstance()->MakeRenderTexture("Play");
	TextureManager::GetInstance()->MakeRenderTexture("Screen");

	//シーンマネージャー生成
	unique_ptr<SceneFactory> sceneFactory = make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory.get());
	shared_ptr<Input> input = make_shared<Input>();
	input->Initialize(GameEngine::GetWindowsAPI());
	SceneManager::GetInstance()->SetInput(input);

	SceneManager::GetInstance()->ChangeScene("Game");
	//SceneManager::GetInstance()->ChangeScene("CourseEditor");

	//ウィンドウの×ボタンが押されるまでループ
	while (GameEngine::WindowState()) {
		if (GameEngine::StartFlame()) {

			//入力処理
			input->Update();

			//
			//	更新処理
			//

			SceneManager::GetInstance()->Update();

#ifdef USE_IMGUI
			ImGui::Begin("Performance");

			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("FPS: %.1f", io.Framerate);
			ImGui::Text("Frame Time: %.3f ms", 1000.0f / io.Framerate);

			ImGui::End();
#endif // DEBUG


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