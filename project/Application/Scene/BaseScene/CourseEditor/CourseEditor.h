#pragma once
#ifdef USE_IMGUI

#include "../BaseScene.h"
#include "GameEngine.h"
#include "Course/Course.h"

class CourseEditor : public BaseScene {
private:

	SRT cameraTransform_{};
	// カメラ
	std::shared_ptr<Camera> defaultCamera_ = nullptr;
	std::unique_ptr<GameCamera> gameCamera_;

	//光源
	DirectionalLightElement directionalLightElement_{};
	std::shared_ptr<DirectionalLight> directionalLight_ = nullptr;

	//選択エリア
	std::unique_ptr<Object> mapchipAreaObject_;
	SRT mapchipAreaTransform_;
	bool isDrawMapchipArea_;
	//配置予定のボックス
	std::unique_ptr<Object> setVoxelObject_;
	SRT setVoxelTransform_;
	bool isSetVoxel_;
	//配置予定のボックス(カーソル上)
	std::unique_ptr<Object> cursorVoxelObject_;
	SRT  cursorVoxelTransform_;
	bool isCursorVoxel_;

	std::unique_ptr<GoalBarrier> barrier_;

	float time_;

	//コース
	std::unique_ptr<Course> course_;

	static const std::string courseDataDirectoryPath_;

	//コースファイルデータ
	CourseData courseData_;

	enum class MenuItem {
		None,
		MakeNewFile,
		OpenFlie,
	};

	//ファイル制作しているか
	MenuItem isOpenFile_ = MenuItem::None;

	enum class ChunkSettingItem {
		None,
		Copy,
		Swap,
	};

	//選択中のチャンク
	Vector3 selectChunk_;
	Vector3 selectChunkSub_;
	//コピー元チャンク位置
	Vector3 copyChunkPos_;

	//チャンク操作の設定
	ChunkSettingItem chunkSettingItem_ = ChunkSettingItem::None;

	//選択中のy座標
	int mapchipAreaY_;
	//選択中のボクセル
	int selectedTile_;

	//描画範囲
	AABB drawAABB_;

public:

	//初期化
	void Initialize(std::shared_ptr<Input> input) override;
	//終了処理
	void Finalize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;

private:

	//新しくコースを作る
	void MakeNewCourse();
	//既存のコースを開く
	void OpenCourse();
	//最後に開いたコースを保持
	void WriteRecentFile();
	//最後に開いたコースを読む
	bool LeadRecentFile();
	//コースを読む
	void LoadCourse(std::string filePath);
	//コースを保存
	void SaveCourse(std::string filePath);

};
#endif // USE_IMGUI