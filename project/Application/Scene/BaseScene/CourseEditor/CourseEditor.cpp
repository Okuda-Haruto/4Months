#define NOMINMAX
#include "CourseEditor.h"

#ifdef USE_IMGUI

const std::string CourseEditor::courseDataDirectoryPath_ = "resources/CourseData";

void CourseEditor::Initialize(std::shared_ptr<Input> input) {

	input_ = input;

	//カメラ
	defaultCamera_ = Object::GetDefaultCamera();
	gameCamera_ = make_unique<GameCamera>();
	gameCamera_->Initialize(defaultCamera_, std::make_unique<EditorCamera>(), input_, nullptr,nullptr);
	gameCamera_->ChangeCamera(std::make_unique<EditorCamera>(), 0);

	//光源
	directionalLight_ = make_shared<DirectionalLight>();
	directionalLight_->Initialize(GameEngine::GetDirectXCommon());
	directionalLightElement_ = {
		{1.0f,1.0f,1.0f,1.0f},
		Normalize(Vector3{0.0f,-1.0f,0.25f}),
		1.0f
	};
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	skydome_ = std::make_unique<Object>();
	skydome_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/GoalBarrier", "Skydome.obj"));
	skydome_->SetReflection(REFLECTION_None);
	skydome_->SetCamera(defaultCamera_);

	mapchipAreaObject_ = std::make_unique<Object>();
	mapchipAreaObject_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/box", "box.obj"));
	mapchipAreaObject_->SetCamera(gameCamera_->GetCamera());
	mapchipAreaObject_->SetReflection(REFLECTION_None);
	mapchipAreaObject_->SetColor(Vector4{ 1.0f,1.0f,1.0f,0.5f });
	mapchipAreaTransform_.scale = Vector3{ 16.0f * 3.0f + 0.1f,3.0f + 0.1f,16.0f * 3.0f + 0.1f };

	setVoxelObject_ = std::make_unique<Object>();
	setVoxelObject_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/Face", "Block.obj"));
	setVoxelObject_->SetCamera(gameCamera_->GetCamera());
	setVoxelObject_->SetReflection(REFLECTION_None);
	setVoxelObject_->SetColor(Vector4{ 1.0f,1.0f,1.0f,0.75f });
	setVoxelTransform_.scale = Vector3{ 3.0f,3.0f,3.0f };
	isSetVoxel_ = false;

	cursorVoxelObject_ = std::make_unique<Object>();
	cursorVoxelObject_->Initialize(ModelManager::GetInstance()->GetModel("resources/Course/Face", "Block.obj"));
	cursorVoxelObject_->SetCamera(gameCamera_->GetCamera());
	cursorVoxelObject_->SetReflection(REFLECTION_None);
	cursorVoxelObject_->SetColor(Vector4{ 1.0f,1.0f,1.0f,0.75f });
	cursorVoxelTransform_.scale = Vector3{ 3.01f,3.01f,3.01f };
	isCursorVoxel_ = false;

	barrier_ = std::make_unique<GoalBarrier>();
	barrier_->Initialize(-60, defaultCamera_);

	player_ = std::make_unique<Player>();
	player_->Initialize(Vector3{ 0,100,0 }, nullptr, nullptr);

	selectChunk_ = {};
	selectedTile_ = TILE_None;

	//ありえない値を入れておく
	copyChunkPos_ = { -1,-1,-1 };
}

void CourseEditor::Finalize() {

}

void CourseEditor::Update() {
	Keyboard key = input_->GetKeyBoard();
	Mouse mouse = input_->GetMouse();

	time_ += GameEngine::GetDeltaTime();

	barrier_->Update(gameCamera_.get());

	std::vector<Parts> parts = skydome_->GetParts();
	parts[0].UVtransform.translate.y += 0.01f;
	skydome_->SetParts(parts[0], 0);

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(340, 720));
	ImGui::Begin("エディターメニュー");
	//コースが選択されていない場合
	if (!course_) {
		if (ImGui::Button("新しくコースを作る")) {
			isOpenFile_ = MenuItem::MakeNewFile;
		}
		if (ImGui::Button("コースファイルを開く")) {
			isOpenFile_ = MenuItem::OpenFlie;
		}
		if (ImGui::Button("最後に開いたコースファイルを開く")) {
			LeadRecentFile();
			OpenCourse();
		}
		ImGui::End();

		//選択した場合場合別のウィンドウを開く
		static char fileName[16] = "";
		static char directoryPath[64] = "";
		static char voxelPath[64] = "";
		float step = 1.0f;
		switch (isOpenFile_)
		{
		case CourseEditor::MenuItem::MakeNewFile:
			ImGui::SetNextWindowPos(ImVec2(340, 260));
			ImGui::SetNextWindowSize(ImVec2(600, 200));

			ImGui::Begin("新しくコースを作る");
			ImGui::Text("ファイル名");
			ImGui::InputText("##ファイル名", fileName,sizeof(fileName));
			ImGui::Text("チャンクディレクトリへのパス");
			ImGui::InputText("##チャンクディレクトリへのパス", directoryPath, sizeof(directoryPath));
			ImGui::Text("チャンク数");
			ImGui::InputScalarN("##チャンク数",
				ImGuiDataType_Float,
				&courseData_.csvData.size.x,
				3,
				&step,
				&step,
				"%.0f"
			);
			ImGui::Text("使用するボクセルデータ");
			ImGui::InputText("##使用するボクセルデータ", voxelPath, sizeof(voxelPath));

			// 右寄せ
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 75);

			if (ImGui::Button("戻る")) {
				isOpenFile_ = MenuItem::None;
			}
			ImGui::SameLine();
			if (ImGui::Button("作成")) {
				courseData_.fileName = fileName;
				courseData_.csvData.chunkDataDirectoryPath = directoryPath;
				courseData_.csvData.voxelDataFilePath = voxelPath;
				MakeNewCourse();
			}
			ImGui::End();
			break;
		case CourseEditor::MenuItem::OpenFlie:
			ImGui::SetNextWindowPos(ImVec2(340, 306));
			ImGui::SetNextWindowSize(ImVec2(600, 108));

			ImGui::Begin("コースファイルを開く");
			ImGui::Text("ファイル名");
			ImGui::InputText("##ファイル名", fileName, sizeof(fileName));

			// 右寄せ
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 75);

			if (ImGui::Button("戻る")) {
				isOpenFile_ = MenuItem::None;
			}
			ImGui::SameLine();
			if (ImGui::Button("開く")) {
				courseData_.fileName = fileName;
				OpenCourse();
			}
			ImGui::End();
			break;
		default:
			break;
		}
	//コースエディター部分
	} else {
		
		course_->Update(player_.get());

		gameCamera_->Update();

#pragma region エディターステータス

		//チャンク描画
		float voxelSize = 3.0f * 2.0f;
		float chunkSize = voxelSize * 16.0f;

		Vector3 chunkOrigin = {
			selectChunk_.x * chunkSize - chunkSize / 2 * courseData_.csvData.size.x,
			-((selectChunk_.y + 1) * chunkSize),
			selectChunk_.z * chunkSize - chunkSize / 2 * courseData_.csvData.size.z
		};

		AABB chunkAABB;
		chunkAABB.min = chunkOrigin;
		chunkAABB.max = chunkOrigin + Vector3{ chunkSize, chunkSize, chunkSize };

		Vector3 chunkOriginSub = {
			selectChunkSub_.x * chunkSize - chunkSize / 2 * courseData_.csvData.size.x,
			-((selectChunkSub_.y + 1) * chunkSize),
			selectChunkSub_.z * chunkSize - chunkSize / 2 * courseData_.csvData.size.z
		};

		AABB chunkAABBSub;
		chunkAABBSub.min = chunkOriginSub;
		chunkAABBSub.max = chunkOriginSub + Vector3{ chunkSize, chunkSize, chunkSize };

		// テクスチャID (DxLibやDirectXから取得したID)
		ImTextureID textureID = (ImTextureID)GameEngine::GetSRVManager()->GetGPUDescriptorHandle(TextureManager::GetInstance()->GetSrvIndex("resources/Course/Face/Brick.png")).ptr;
		ImVec2 uv00 = ImVec2(0.0f, 0.0f); // タイルのUV開始位置
		ImVec2 uv11 = ImVec2(0.0f, 1.0f); // タイルのUV終了位置 (例: 4x4のタイルセットの左上)

#pragma endregion

#pragma region コース設定

		if (ImGui::TreeNode("コース設定")) {

			if (ImGui::Button("コースサイズ変更")) {
				copyCourseSize_ = courseData_.csvData.size;
				chunkSettingItem_ = ChunkSettingItem::Resize;
			}

			if (ImGui::Button("セクション追加")) {
				sectionSettingItem_ = SectionSettingItem::Add;
			}
			if (ImGui::Button("セクション変更")) {
				sectionSettingItem_ = SectionSettingItem::Resize;
			}

			ImGui::TreePop();
		}

#pragma endregion

#pragma region チャンク操作

		PrimitiveManager::GetInstance()->AddAABB(chunkAABB);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

		if (ImGui::TreeNode("チャンク操作")) {

			ImGui::Text("チャンク選択");
			ImGui::SliderFloat("X##1", &selectChunk_.x, 0, courseData_.csvData.size.x - 1, "%.0f");
			ImGui::SliderFloat("Y##1", &selectChunk_.y, 0, courseData_.csvData.size.y - 1, "%.0f");
			ImGui::SliderFloat("Z##1", &selectChunk_.z, 0, courseData_.csvData.size.z - 1, "%.0f");

			if (ImGui::Button("チャンク横回転(Y軸回転)")) {
				course_->GetVoxel()->ChunkHorizontalRotation(selectChunk_);
			}
			if (ImGui::Button("チャンク縦回転(X軸回転)")) {
				course_->GetVoxel()->ChunkVerticalRotation(selectChunk_);
			}
			if (ImGui::Button("チャンク鏡面反転")) {
				course_->GetVoxel()->ChunkMirror(selectChunk_);
			}

			if (ImGui::Button("チャンクをコピー")) {
				chunkSettingItem_ = ChunkSettingItem::Copy;
			}
			if (ImGui::Button("チャンクを交換")) {
				chunkSettingItem_ = ChunkSettingItem::Swap;
			}
			if (ImGui::Button("チャンクを上に差し込む")) {
				course_->GetVoxel()->AddChunkY(int(selectChunk_.y));
				selectChunk_.y += 1.0f;
				courseData_.csvData.size.y += 1.0f;
			}

			ImGui::TreePop();
		}

#pragma endregion

#pragma region ボクセル操作
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

		isDrawMapchipArea_ = false;

		if (ImGui::TreeNode("ボクセル操作")) {
			isDrawMapchipArea_ = true;

			ImGui::SliderInt("高度", &mapchipAreaY_, 0, 15);

			mapchipAreaTransform_.translate = chunkOrigin + Vector3{ chunkSize / 2, chunkSize - mapchipAreaY_ * voxelSize - voxelSize / 2, chunkSize / 2 };
			mapchipAreaObject_->SetTransform(mapchipAreaTransform_);

			std::vector<std::vector<std::vector<Chunk>>> chunks_ = course_->GetVoxel()->GetChunks();

			ImGui::Text("マップチップ 16 * 16");

			isSetVoxel_ = false;

			for (int z = 15; z >= 0; z--) {
				for (int x = 0; x < 16; x++) {
					std::string id = "##tile" + std::to_string(z) + "_" + std::to_string(x);

					//TILEの見た目変更
					if (chunks_[int(selectChunk_.y)][int(selectChunk_.z)][int(selectChunk_.x)].mapChip[mapchipAreaY_][z][x] != TILE_None) {
						uv00.x = float(chunks_[int(selectChunk_.y)][int(selectChunk_.z)][int(selectChunk_.x)].mapChip[mapchipAreaY_][z][x] - 1) / int(VOXEL_TILE_END - 1);
					} else {
						uv00.x = 0.0f;
					}
					uv11.x = float(chunks_[int(selectChunk_.y)][int(selectChunk_.z)][int(selectChunk_.x)].mapChip[mapchipAreaY_][z][x]) / int(VOXEL_TILE_END - 1);

					//隙間消し用
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

					// クリック可能なマップチップボタン
					if (ImGui::ImageButton(id.c_str(), textureID, ImVec2(16, 16), uv00, uv11)) {
						// タイルが選択された時の処理
						chunks_[int(selectChunk_.y)][int(selectChunk_.z)][int(selectChunk_.x)].mapChip[mapchipAreaY_][z][x] = selectedTile_;
						course_->GetVoxel()->SetVoxel(selectChunk_, mapchipAreaY_, z, x, chunks_[int(selectChunk_.y)][int(selectChunk_.z)][int(selectChunk_.x)].mapChip[mapchipAreaY_][z][x]);
					}

					//ホバー中か
					if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
						isSetVoxel_ = true;
						setVoxelTransform_.translate = chunkOrigin + Vector3{ voxelSize * x + voxelSize / 2, chunkSize - mapchipAreaY_ * voxelSize - voxelSize / 2, voxelSize * z + voxelSize / 2 };
						setVoxelObject_->SetTransform(setVoxelTransform_);
						if (selectedTile_ > 0) {
							std::vector<Parts> parts = setVoxelObject_->GetParts();
							parts[0].UVtransform.scale.x = 1.0f / (VOXEL_TILE_END - 1);
							parts[0].UVtransform.translate.x = (1.0f / (VOXEL_TILE_END - 1)) * (selectedTile_ - 1);
							setVoxelObject_->SetParts(parts[0], 0);
						} else {
							std::vector<Parts> parts = setVoxelObject_->GetParts();
							parts[0].UVtransform.scale.x = 0.0000001f;
							parts[0].UVtransform.translate.x = 0;
							setVoxelObject_->SetParts(parts[0], 0);
						}
					}

					// グリッドを並べる
					if (x < 15) ImGui::SameLine();

					ImGui::PopStyleVar(2);
				}
			}

			if (ImGui::Button("上のマップチップをコピー")) {
				course_->GetVoxel()->CopyUpperMapChip(selectChunk_, mapchipAreaY_);
			}
			if (ImGui::Button("下のマップチップをコピー")) {
				course_->GetVoxel()->CopyUnderMapChip(selectChunk_, mapchipAreaY_);
			}

			ImGui::TreePop();
		}

#pragma endregion

#pragma region 描画範囲

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

		if (ImGui::TreeNode("描画範囲")) {

			ImGui::Text("min");
			ImGui::SliderFloat("X##3", &drawAABB_.min.x, 0, courseData_.csvData.size.x - 1, "%.0f");
			ImGui::SliderFloat("Y##3", &drawAABB_.min.y, 0, courseData_.csvData.size.y - 1, "%.0f");
			ImGui::SliderFloat("Z##3", &drawAABB_.min.z, 0, courseData_.csvData.size.z - 1, "%.0f");
			ImGui::Text("max");
			ImGui::SliderFloat("X##4", &drawAABB_.max.x, 0, courseData_.csvData.size.x - 1, "%.0f");
			ImGui::SliderFloat("Y##4", &drawAABB_.max.y, 0, courseData_.csvData.size.y - 1, "%.0f");
			ImGui::SliderFloat("Z##4", &drawAABB_.max.z, 0, courseData_.csvData.size.z - 1, "%.0f");

			if (ImGui::Button("1チャンク下げる")) {
				drawAABB_.min.y = std::min(drawAABB_.min.y + 1.0f, courseData_.csvData.size.y - 1.0f);
				drawAABB_.max.y = std::min(drawAABB_.max.y + 1.0f, courseData_.csvData.size.y - 1.0f);
			}
			if (ImGui::Button("1チャンク上げる")) {
				drawAABB_.min.y = std::max(drawAABB_.min.y - 1.0f, 0.0f);
				drawAABB_.max.y = std::max(drawAABB_.max.y - 1.0f, 0.0f);
			}
			if (ImGui::Button("1チャンク下に広げる")) {
				drawAABB_.max.y = std::min(drawAABB_.max.y + 1.0f, courseData_.csvData.size.y - 1.0f);
			}
			if (ImGui::Button("1チャンク上に広げる")) {
				drawAABB_.min.y = std::max(drawAABB_.min.y - 1.0f, 0.0f);
			}

			ImGui::TreePop();
		}

		drawAABB_.min.x = std::min(drawAABB_.min.x, drawAABB_.max.x + 1);
		drawAABB_.min.y = std::min(drawAABB_.min.y, drawAABB_.max.y + 1);
		drawAABB_.min.z = std::min(drawAABB_.min.z, drawAABB_.max.z + 1);

		drawAABB_.max.x = std::max(drawAABB_.min.x + 1, drawAABB_.max.x);
		drawAABB_.max.y = std::max(drawAABB_.min.y + 1, drawAABB_.max.y);
		drawAABB_.max.z = std::max(drawAABB_.min.z + 1, drawAABB_.max.z);

#pragma endregion

#pragma region SAVE

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

		if (ImGui::Button("SAVE")) {
			course_->GetVoxel()->Save(courseData_.csvData.chunkDataDirectoryPath);
			SaveCourse(courseDataDirectoryPath_ + "/" + courseData_.fileName + ".csv");
		}

#pragma endregion

#pragma region マニュアル

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

		ImGui::Text("[マニュアル]");

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

		ImGui::Text("ホイール回転 : カメラ上下移動");
		ImGui::Text("右クリック + 移動 : カメラ回転");
		ImGui::Text("右クリック + ホイール回転 : カメラ前後移動");
		ImGui::Text("ホイールクリック + 移動 : カメラ位置調整");
		ImGui::Text("左クリック : ボクセル配置 + チャンクカーソルを合わせる");
		ImGui::Text("*下面は現在描画していません。");
		ImGui::Text("*ボクセルの予測が出ていない場合は埋っています。");
		ImGui::Text(" その状態で配置すると意図しない場所に配置されます。");

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

		ImGui::Text("Ctrl + C : 現在のチャンクをコピー");
		ImGui::Text("Ctrl + V : 現在のチャンクにペースト");
		ImGui::Text("PGUP,PGDN : チャンクカーソル上下移動");
		ImGui::Text("矢印キー : チャンクカーソル前後左右移動");
		ImGui::Text("Q : チャンク回転");
		ImGui::Text("W,S : 描画範囲上下移動");
		ImGui::Text("Ctrl + W,S : 描画範囲上下拡大");
		ImGui::Text("Ctrl + R : 初期化");
		ImGui::Text("*SAVEしていない編集は反映されないので注意してください。");

#pragma endregion


		ImGui::End();

#pragma region TILEパレット
		ImGui::Begin("タイルパレット");
		for (int i = 0; i < int(VOXEL_TILE_END);i++) {
			std::string id = "##pallet" + std::to_string(i);
			//TILEの見た目変更
			if (i != TILE_None) {
				uv00.x = float(i - 1) / int(VOXEL_TILE_END - 1);
			} else {
				uv00.x = 0.0f;
			}
			uv11.x = float(i) / int(VOXEL_TILE_END - 1);

			//クリックでselectedTile_が変わるので状態を保持
			bool isSelected = (i == selectedTile_);

			if (isSelected) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.0f, 0.0f, 1));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.1f, 0.1f, 1));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1));
			} else {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1));
			}

			//隙間消し用
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

			// クリック可能なマップチップボタン
			if (ImGui::ImageButton(id.c_str(), textureID, ImVec2(64, 64), uv00, uv11)) {
				selectedTile_ = i;
			}

			if (i < VOXEL_TILE_END - 1 && ((i + 1) % 5 != 0) )ImGui::SameLine();

			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor(3);
		}

		//空気以外ならステータスを変更可能に
		if (selectedTile_ != 0) {
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

			courseData_.csvData.voxelStatus = course_->GetVoxel()->GetVoxelData();
			if (courseData_.csvData.voxelStatus.size() <= selectedTile_) {
				courseData_.csvData.voxelStatus.resize(selectedTile_ + 1);
			}

			ImGui::DragFloat("耐久度",&courseData_.csvData.voxelStatus[selectedTile_].MaxHP,0.1f,0.0f,180.0f);
			ImGui::DragFloat("耐久度乱数範囲", &courseData_.csvData.voxelStatus[selectedTile_].randomRate, 0.1f, 0.0f, 180.0f);
			ImGui::DragFloat("巻き込まれやすさ", &courseData_.csvData.voxelStatus[selectedTile_].vacuumSensitivity, 0.01f, 0.0f, 1.0f);

			course_->GetVoxel()->SetVoxelData(courseData_.csvData.voxelStatus);
		}


		ImGui::End();
#pragma endregion

#pragma region Chunkコピー
		//チャンクコピーか入れ替えを選択した場合場合別のウィンドウを開く
		static char fileName[16] = "";
		static char directoryPath[64] = "";
		float step = 1.0f;
		switch (chunkSettingItem_)
		{
		case CourseEditor::ChunkSettingItem::Copy:
			ImGui::SetNextWindowSize(ImVec2(600, 256));

			ImGui::Begin("チャンクをコピー");
			ImGui::Text("コピー元");
			ImGui::SliderFloat("X##1", &selectChunk_.x, 0, courseData_.csvData.size.x - 1, "%.0f");
			ImGui::SliderFloat("Y##1", &selectChunk_.y, 0, courseData_.csvData.size.y - 1, "%.0f");
			ImGui::SliderFloat("Z##1", &selectChunk_.z, 0, courseData_.csvData.size.z - 1, "%.0f");
			ImGui::Text("コピー先");
			ImGui::SliderFloat("X##2", &selectChunkSub_.x, 0, courseData_.csvData.size.x - 1, "%.0f");
			ImGui::SliderFloat("Y##2", &selectChunkSub_.y, 0, courseData_.csvData.size.y - 1, "%.0f");
			ImGui::SliderFloat("Z##2", &selectChunkSub_.z, 0, courseData_.csvData.size.z - 1, "%.0f");

			// 右寄せ
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 85);

			if (ImGui::Button("戻る")) {
				chunkSettingItem_ = ChunkSettingItem::None;
			}
			ImGui::SameLine();
			if (ImGui::Button("コピー")) {
				course_->GetVoxel()->ChunkCopy(selectChunk_, selectChunkSub_);
			}
			ImGui::End();

			PrimitiveManager::GetInstance()->AddAABB(chunkAABBSub, Vector4{ 0,1,0,1 });

			break;
		case CourseEditor::ChunkSettingItem::Swap:
			ImGui::SetNextWindowSize(ImVec2(600, 256));

			ImGui::Begin("チャンクを交換");
			ImGui::Text("交換元");
			ImGui::SliderFloat("X##1", &selectChunk_.x, 0, courseData_.csvData.size.x - 1, "%.0f");
			ImGui::SliderFloat("Y##1", &selectChunk_.y, 0, courseData_.csvData.size.y - 1, "%.0f");
			ImGui::SliderFloat("Z##1", &selectChunk_.z, 0, courseData_.csvData.size.z - 1, "%.0f");
			ImGui::Text("交換先");
			ImGui::SliderFloat("X##2", &selectChunkSub_.x, 0, courseData_.csvData.size.x - 1, "%.0f");
			ImGui::SliderFloat("Y##2", &selectChunkSub_.y, 0, courseData_.csvData.size.y - 1, "%.0f");
			ImGui::SliderFloat("Z##2", &selectChunkSub_.z, 0, courseData_.csvData.size.z - 1, "%.0f");

			// 右寄せ
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 85);

			if (ImGui::Button("戻る")) {
				chunkSettingItem_ = ChunkSettingItem::None;
			}
			ImGui::SameLine();
			if (ImGui::Button("交換")) {
				course_->GetVoxel()->ChunkSwap(selectChunk_, selectChunkSub_);
			}
			ImGui::End();

			PrimitiveManager::GetInstance()->AddAABB(chunkAABBSub, Vector4{0,1,0,1});

			break;
		case CourseEditor::ChunkSettingItem::Resize:
			ImGui::SetNextWindowSize(ImVec2(600, 128));

			ImGui::Begin("チャンクサイズ変更");
			ImGui::Text("チャンク数");
			ImGui::InputScalarN("##チャンク数",
				ImGuiDataType_Float,
				&copyCourseSize_.x,
				3,
				&step,
				&step,
				"%.0f"
			);


			if (ImGui::Button("戻る")) {
				chunkSettingItem_ = ChunkSettingItem::None;
			}
			ImGui::SameLine();
			if (ImGui::Button("変更")) {
				courseData_.csvData.size = copyCourseSize_;
				course_->GetVoxel()->Resize(courseData_.csvData.size);
				drawAABB_.min.x = 0;						drawAABB_.min.z = 0;
				drawAABB_.max.x = courseData_.csvData.size.x;	drawAABB_.max.z = courseData_.csvData.size.z;
				chunkSettingItem_ = ChunkSettingItem::None;
			}

			ImGui::End();
			break;
		default:
			break;
		}
#pragma endregion

#pragma region Setcionコピー

		static int sectionIndex = 0;
		switch (sectionSettingItem_)
		{
		case SectionSettingItem::Add:
			ImGui::SetNextWindowSize(ImVec2(600, 446));

			ImGui::Begin("セクションを追加");
			ImGui::Text("セクションチャンク");
			ImGui::SliderInt("開始地点", &sectionData_.startChunkY, 0, int(courseData_.csvData.size.y) - 1);
			ImGui::SliderInt("終了地点", &sectionData_.endChunkY, 0, int(courseData_.csvData.size.y) - 1);
			ImGui::Text("最大時間");
			ImGui::DragFloat("##Time", &sectionData_.maxSeconds);
			ImGui::Text("制限スコア");
			ImGui::DragInt("クリア", &sectionData_.clearScore,100);
			ImGui::DragInt("最大", &sectionData_.maxScore, 100);
			ImGui::Text("Aランクライン");
			ImGui::DragInt("破壊割合##A", &sectionData_.rankBorders.rate.aScore, 1,0,100);
			ImGui::DragInt("破壊スコア##A", &sectionData_.rankBorders.count.aScore, 100);
			ImGui::DragInt("クリア時間##A", &sectionData_.rankBorders.time.aScore, 1, 0, int(sectionData_.maxSeconds));
			ImGui::Text("Bランクライン");
			ImGui::DragInt("破壊割合##B", &sectionData_.rankBorders.rate.bScore, 1, 0, 100);
			ImGui::DragInt("破壊スコア##B", &sectionData_.rankBorders.count.bScore, 100);
			ImGui::DragInt("クリア時間##B", &sectionData_.rankBorders.time.bScore, 1, 0, int(sectionData_.maxSeconds));

			// 右寄せ
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 85);

			if (ImGui::Button("戻る")) {
				sectionSettingItem_ = SectionSettingItem::None;
			}
			ImGui::SameLine();
			if (ImGui::Button("追加")) {
				courseData_.sectionDatas.push_back(sectionData_);
				course_->AddSection(sectionData_.startChunkY, sectionData_.endChunkY, sectionData_.maxSeconds, sectionData_.clearScore, sectionData_.maxScore, sectionData_.rankBorders);
				sectionSettingItem_ = SectionSettingItem::None;

				course_->ResetGoalBarrier();
			}
			ImGui::End();

			break;
		case SectionSettingItem::Resize:
			ImGui::SetNextWindowSize(ImVec2(600, 472));

			ImGui::Begin("セクション変更");
			ImGui::SliderInt("セクション番号", &sectionIndex, 0, int(courseData_.sectionDatas.size() - 1));
			ImGui::Text("セクションチャンク");
			ImGui::SliderInt("開始地点", &courseData_.sectionDatas[sectionIndex].startChunkY, 0, int(courseData_.csvData.size.y) - 1);
			ImGui::SliderInt("終了地点", &courseData_.sectionDatas[sectionIndex].endChunkY, 0, int(courseData_.csvData.size.y) - 1);
			ImGui::Text("最大時間");
			ImGui::DragFloat("##Time", &courseData_.sectionDatas[sectionIndex].maxSeconds);
			ImGui::Text("制限スコア");
			ImGui::DragInt("クリア", &courseData_.sectionDatas[sectionIndex].clearScore, 100);
			ImGui::DragInt("最大", &courseData_.sectionDatas[sectionIndex].maxScore, 100);
			ImGui::Text("Aランクライン");
			ImGui::DragInt("破壊割合##A", &courseData_.sectionDatas[sectionIndex].rankBorders.rate.aScore, 1, 0, 100);
			ImGui::DragInt("破壊スコア##A", &courseData_.sectionDatas[sectionIndex].rankBorders.count.aScore, 100);
			ImGui::DragInt("クリア時間##A", &courseData_.sectionDatas[sectionIndex].rankBorders.time.aScore, 1, 0, int(courseData_.sectionDatas[sectionIndex].maxSeconds));
			ImGui::Text("Bランクライン");
			ImGui::DragInt("破壊割合##B", &courseData_.sectionDatas[sectionIndex].rankBorders.rate.bScore, 1, 0, 100);
			ImGui::DragInt("破壊スコア##B", &courseData_.sectionDatas[sectionIndex].rankBorders.count.bScore, 100);
			ImGui::DragInt("クリア時間##B", &courseData_.sectionDatas[sectionIndex].rankBorders.time.bScore, 1, 0, int(courseData_.sectionDatas[sectionIndex].maxSeconds));

			// 右寄せ
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 85);

			if (ImGui::Button("戻る")) {
				sectionSettingItem_ = SectionSettingItem::None;
			}
			ImGui::SameLine();
			if (ImGui::Button("変更")) {
				//ステージエディターだとサイズ以外関係ない
				course_->SetSectionChunkSize(courseData_.sectionDatas[sectionIndex].startChunkY, courseData_.sectionDatas[sectionIndex].endChunkY, sectionIndex);
				sectionSettingItem_ = SectionSettingItem::None;

				course_->ResetGoalBarrier();
			}
			ImGui::End();
			break;
		default:
			break;
		}
#pragma endregion

#pragma region マウス配置

		if (!ImGui::IsAnyItemActive() &&
			!ImGui::IsAnyItemHovered() &&
			!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {

			// ビューポート行列
			Matrix4x4 matViewport = MakeViewportMatrix(0, 0, float(GameEngine::GetWindowWidth()), float(GameEngine::GetWindowHeight()), 0, 1);

			// ビュー行列とプロジェクション行列、ビューポート行列を合成する
			Matrix4x4 matViewProjectionViewport = gameCamera_->GetCamera()->GetViewMatrix() * gameCamera_->GetCamera()->GetProjectionMatrix() * matViewport;

			//合成行列の逆行列を計算する
			Matrix4x4 matInverseVPV = Inverse(matViewProjectionViewport);

			//スクリーン座標
			Vector3 posNear = Vector3(mouse.Position.x, mouse.Position.y, 0.0f);
			Vector3 posFar = Vector3(mouse.Position.x, mouse.Position.y, 1.0f);

			//スクリーン座標系からワールド座標系へ
			posNear = Transform(posNear, matInverseVPV);
			posFar = Transform(posFar, matInverseVPV);

			//マウスレイの方向
			Vector3 mouseDirection = posFar - posNear;
			mouseDirection = Normalize(mouseDirection);

			//カメラから照準オブジェクトの距離	モデルがカメラから50離れているので更に50離す
			const float kDistanceTestObject = 100.0f;
			Vector3 rayDir = posNear + mouseDirection - gameCamera_->GetTransform().translate;

			Vector3 rayOrigin = gameCamera_->GetTransform().translate;

			Ray ray = {
				gameCamera_->GetTransform().translate,
				rayDir
			};

			CollisionVoxel collisionVoxel = course_->GetVoxel()->GetCollisionVoxel(ray, drawAABB_);

			//レイが接触していたら
			if (Length(collisionVoxel.normal) > 0.000001f) {

				isCursorVoxel_ = true;

				//接触した面だけ動かす
				if (selectedTile_ > 0) {

					collisionVoxel.mapChipPos += Vector3{ collisionVoxel.normal.x, -collisionVoxel.normal.y, collisionVoxel.normal.z };

					if (collisionVoxel.mapChipPos.x < 0) {
						collisionVoxel.chunkPos.x -= 1;
						collisionVoxel.mapChipPos.x += 16;
						if (collisionVoxel.chunkPos.x < 0) {
							isCursorVoxel_ = false;
						}
					} else if (collisionVoxel.mapChipPos.x > 15) {
						collisionVoxel.chunkPos.x += 1;
						collisionVoxel.mapChipPos.x -= 16;
						if (collisionVoxel.chunkPos.x > courseData_.csvData.size.x - 1) {
							isCursorVoxel_ = false;
						}
					}
					if (collisionVoxel.mapChipPos.y < 0) {
						collisionVoxel.chunkPos.y -= 1;
						collisionVoxel.mapChipPos.y += 16;
						if (collisionVoxel.chunkPos.y < 0) {
							isCursorVoxel_ = false;
						}
					} else if (collisionVoxel.mapChipPos.y > 15) {
						collisionVoxel.chunkPos.y += 1;
						collisionVoxel.mapChipPos.y -= 16;
						if (collisionVoxel.chunkPos.y > courseData_.csvData.size.y - 1) {
							isCursorVoxel_ = false;
						}
					}
					if (collisionVoxel.mapChipPos.z < 0) {
						collisionVoxel.chunkPos.z -= 1;
						collisionVoxel.mapChipPos.z += 16;
						if (collisionVoxel.chunkPos.z < 0) {
							isCursorVoxel_ = false;
						}
					} else if (collisionVoxel.mapChipPos.z > 15) {
						collisionVoxel.chunkPos.z += 1;
						collisionVoxel.mapChipPos.z -= 16;
						if (collisionVoxel.chunkPos.z > courseData_.csvData.size.z - 1) {
							isCursorVoxel_ = false;
						}
					}

					std::vector<Parts> parts = cursorVoxelObject_->GetParts();
					parts[0].UVtransform.scale.x = 1.0f / (VOXEL_TILE_END - 1);
					parts[0].UVtransform.translate.x = (1.0f / (VOXEL_TILE_END - 1)) * (selectedTile_ - 1);
					cursorVoxelObject_->SetParts(parts[0], 0);
					//何もないを選択してるならそのまま
				} else {

					std::vector<Parts> parts = cursorVoxelObject_->GetParts();
					parts[0].UVtransform.scale.x = 0.0000001f;
					parts[0].UVtransform.translate.x = 0;
					cursorVoxelObject_->SetParts(parts[0], 0);
				}

				Vector3 collisionChunkOrigin = {
					collisionVoxel.chunkPos.x * chunkSize - chunkSize / 2 * courseData_.csvData.size.x,
					-((collisionVoxel.chunkPos.y + 1) * chunkSize),
					collisionVoxel.chunkPos.z * chunkSize - chunkSize / 2 * courseData_.csvData.size.z
				};

				cursorVoxelTransform_.translate =
					collisionChunkOrigin + Vector3{
					voxelSize * (collisionVoxel.mapChipPos.x) + voxelSize / 2,
					chunkSize - (collisionVoxel.mapChipPos.y) * voxelSize - voxelSize / 2,
					voxelSize * (collisionVoxel.mapChipPos.z) + voxelSize / 2
				};
				cursorVoxelObject_->SetTransform(cursorVoxelTransform_);

				if (isCursorVoxel_ && mouse.click[MOUSE_BOTTON_LEFT].trigger) {
					selectChunk_ = collisionVoxel.chunkPos;
					mapchipAreaY_ = int(collisionVoxel.mapChipPos.y);

					std::vector<std::vector<std::vector<Chunk>>> chunks_ = course_->GetVoxel()->GetChunks();
					// タイルが選択された時の処理
					chunks_[int(selectChunk_.y)][int(selectChunk_.z)][int(selectChunk_.x)].mapChip[int(collisionVoxel.mapChipPos.y)][int(collisionVoxel.mapChipPos.z)][int(collisionVoxel.mapChipPos.x)] = selectedTile_;
					course_->GetVoxel()->SetVoxel(selectChunk_, int(collisionVoxel.mapChipPos.y), int(collisionVoxel.mapChipPos.z), int(collisionVoxel.mapChipPos.x), chunks_[int(selectChunk_.y)][int(selectChunk_.z)][int(selectChunk_.x)].mapChip[int(collisionVoxel.mapChipPos.y)][int(collisionVoxel.mapChipPos.z)][int(collisionVoxel.mapChipPos.x)]);
				}
			} else {
				isCursorVoxel_ = false;
			}
		} else {
			isCursorVoxel_ = false;
		}

#pragma endregion

#pragma region ショートカット

		//コピー
		if ((key.hold[DIK_LCONTROL] || key.hold[DIK_RCONTROL]) && key.trigger[DIK_C]) {
			copyChunkPos_ = selectChunk_;
		}
		//ペースト
		if ((key.hold[DIK_LCONTROL] || key.hold[DIK_RCONTROL]) && key.trigger[DIK_V]) {
			if (copyChunkPos_.x >= 0.0f) {
				course_->GetVoxel()->ChunkCopy(copyChunkPos_, selectChunk_);
			}
		}

		//チャンク位置
		if (key.trigger[DIK_PGUP]) {
			selectChunk_.y = std::max(selectChunk_.y - 1, 0.0f);
		}
		if (key.trigger[DIK_PGDN]) {
			selectChunk_.y = std::min(selectChunk_.y + 1, courseData_.csvData.size.y - 1);
		}
		if (key.trigger[DIK_UPARROW]) {
			selectChunk_.z = std::min(selectChunk_.z + 1, courseData_.csvData.size.z - 1);
		}
		if (key.trigger[DIK_DOWNARROW]) {
			selectChunk_.z = std::max(selectChunk_.z - 1, 0.0f);
		}
		if (key.trigger[DIK_RIGHTARROW]) {
			selectChunk_.x = std::min(selectChunk_.x + 1, courseData_.csvData.size.x - 1);
		}
		if (key.trigger[DIK_LEFTARROW]) {
			selectChunk_.x = std::max(selectChunk_.x - 1, 0.0f);
		}
		//チャンク回転
		if (key.trigger[DIK_Q]) {
			course_->GetVoxel()->ChunkHorizontalRotation(selectChunk_);
		}
		//カメラ拡大＆移動
		if ((key.hold[DIK_LSHIFT] || key.hold[DIK_RSHIFT]) && key.trigger[DIK_W]) {
			drawAABB_.min.y = std::max(drawAABB_.min.y - 1.0f, 0.0f);
		} else if (key.trigger[DIK_W]) {
			drawAABB_.min.y = std::max(drawAABB_.min.y - 1.0f, 0.0f);
			drawAABB_.max.y = std::max(drawAABB_.max.y - 1.0f, 0.0f);
		}
		if ((key.hold[DIK_LSHIFT] || key.hold[DIK_RSHIFT]) && key.trigger[DIK_S]) {
			drawAABB_.max.y = std::min(drawAABB_.max.y + 1.0f, courseData_.csvData.size.y - 1.0f);
		} else if (key.trigger[DIK_S]) {
			drawAABB_.min.y = std::min(drawAABB_.min.y + 1.0f, courseData_.csvData.size.y - 1.0f);
			drawAABB_.max.y = std::min(drawAABB_.max.y + 1.0f, courseData_.csvData.size.y - 1.0f);
		}
		if ((key.hold[DIK_LSHIFT] || key.hold[DIK_RSHIFT]) && key.trigger[DIK_R]) {
			OpenCourse();
		}
	}

#pragma endregion

	GameEngine::RenderPreDraw("BackGround");

	skydome_->Draw3DNoFog();

	GameEngine::RenderPostDraw("BackGround");

}

void CourseEditor::Draw() {
	//背景描画
	GameEngine::DrawScreen(TextureManager::GetInstance()->GetSrvIndex("BackGround"));

	if (course_) {
		course_->Draw(drawAABB_,directionalLight_);

		if (isSetVoxel_) {
			setVoxelObject_->Draw3D();
		}

		if (isCursorVoxel_) {
			cursorVoxelObject_->Draw3D();
		}

		if (isDrawMapchipArea_) {
			mapchipAreaObject_->Draw3D();
		}
	}
}

void CourseEditor::MakeNewCourse() {
	SaveCourse(courseDataDirectoryPath_ + "/" + courseData_.fileName + ".csv");

	if (courseData_.sectionDatas.size() <= 0) {
		SectionData data{};
		courseData_.sectionDatas.push_back(data);
	}

	course_ = std::make_unique<Course>();
	course_->Initialize(courseData_, gameCamera_.get(), directionalLight_);

	float courseBottom = -32 * float(course_->GetVoxel()->GetChunks().size() + 1) * 3.0f + 16.0f * 3.0f;
	gameCamera_->SetCameraPosBottom(courseBottom);

	drawAABB_.min = { 0,0,0 };
	drawAABB_.max = { courseData_.csvData.size.x,min(courseData_.csvData.size.y,3.0f),courseData_.csvData.size.x };

	WriteRecentFile();
}

void CourseEditor::OpenCourse() {
	LoadCourse(courseDataDirectoryPath_ + "/" + courseData_.fileName + ".csv");

	if (courseData_.sectionDatas.size() <= 0) {
		SectionData data{};
		courseData_.sectionDatas.push_back(data);
	}

	course_ = std::make_unique<Course>();
	course_->Initialize(courseData_, gameCamera_.get(), directionalLight_);

	float courseBottom = -32 * float(course_->GetVoxel()->GetChunks().size() + 1) * 3.0f + 16.0f * 3.0f;
	gameCamera_->SetCameraPosBottom(courseBottom);

	drawAABB_.min = { 0,0,0 };
	drawAABB_.max = { courseData_.csvData.size.x,min(courseData_.csvData.size.y,3.0f),courseData_.csvData.size.x };

	WriteRecentFile();
}

void CourseEditor::WriteRecentFile() {
	std::ofstream file("./RecentFile.csv");
	assert(file.is_open());

	file << courseData_.fileName;

	file.close();
}

bool CourseEditor::LeadRecentFile() {
	std::ifstream file("./RecentFile.csv");
	if (!file.is_open()) {
		return false;
	}

	std::getline(file, courseData_.fileName);

	file.close();

	return true;
}

void CourseEditor::LoadCourse(std::string filePath) {
	std::ifstream file(filePath);
	assert(file.is_open());

	// 1行分の文字列を入れる変数
	std::string line;

	while (std::getline(file, line)) {

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		std::getline(line_stream, word, ',');

		// コメント
		if (word.find("//") == 0) {
			continue;
		}

		// カンマ区切りで読む
		if (word.find("ChunkSize") == 0) {
			std::getline(line_stream, word, ',');
			courseData_.csvData.size.x = stof(word);
			std::getline(line_stream, word, ',');
			courseData_.csvData.size.y = stof(word);
			std::getline(line_stream, word, ',');
			courseData_.csvData.size.z = stof(word);
		}

		if (word.find("ChunkDataDirectoryPath") == 0) {
			std::getline(line_stream, word, ',');
			courseData_.csvData.chunkDataDirectoryPath = word;
		}

		if (word.find("VoxelDataFilePath") == 0) {
			std::getline(line_stream, word, ',');
			courseData_.csvData.voxelDataFilePath = word;
		}

		if (word.find("Section") == 0) {
			SectionData section;
			std::getline(line_stream, word, ',');
			section.startChunkY = stoi(word);
			std::getline(line_stream, word, ',');
			section.endChunkY = stoi(word);
			std::getline(line_stream, word, ',');
			section.maxSeconds = stof(word);
			std::getline(line_stream, word, ',');
			section.clearScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.maxScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.rankBorders.rate.aScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.rankBorders.rate.bScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.rankBorders.count.aScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.rankBorders.count.bScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.rankBorders.time.aScore = stoi(word);
			std::getline(line_stream, word, ',');
			section.rankBorders.time.bScore = stoi(word);

			courseData_.sectionDatas.push_back(section);
		}
	}

	file.close();
}

void CourseEditor::SaveCourse(std::string filePath) {
	std::ofstream file(filePath);
	assert(file.is_open());

	file << "ChunkSize" << "," << courseData_.csvData.size.x << "," << courseData_.csvData.size.y << "," << courseData_.csvData.size.z << '\n';
	file << "ChunkDataDirectoryPath" << "," << courseData_.csvData.chunkDataDirectoryPath << '\n';
	file << "VoxelDataFilePath" << "," << courseData_.csvData.voxelDataFilePath << '\n';
	for (int i = 0; i < courseData_.sectionDatas.size(); i++) {
		file << "Section"
			<< "," << courseData_.sectionDatas[i].startChunkY
			<< "," << courseData_.sectionDatas[i].endChunkY
			<< "," << courseData_.sectionDatas[i].maxSeconds
			<< "," << courseData_.sectionDatas[i].clearScore
			<< "," << courseData_.sectionDatas[i].maxScore
			<< "," << courseData_.sectionDatas[i].rankBorders.rate.aScore << "," << courseData_.sectionDatas[i].rankBorders.rate.bScore
			<< "," << courseData_.sectionDatas[i].rankBorders.count.aScore << "," << courseData_.sectionDatas[i].rankBorders.count.bScore
			<< "," << courseData_.sectionDatas[i].rankBorders.time.aScore << "," << courseData_.sectionDatas[i].rankBorders.time.bScore
			<< '\n';
	}

	file.close();
}

#endif // USE_IMGUI