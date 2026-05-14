#pragma once
#include <GameEngine.h>
#include <Shape/Sphere.h>
#include "Box/Box.h"
#include "GameCamera/GameCamera.h"

struct CollisionVoxel {
	Vector3 normal;
	Vector3 chunkPos;
	Vector3 mapChipPos;
	Vector3 translate;
};

enum VOXEL_TILE {
	TILE_None,
	TILE_TEST_01,
	TILE_TEST_02,

	VOXEL_TILE_END,
};

class Course;

struct Chunk {
	std::array<std::array<std::array<uint8_t, 16>, 16>, 16> mapChip;
};

struct VoxelStatus {
	//どれだけ吸引されやすいか
	float vacuumSensitivity;
	//最大耐久度
	float MaxHP;
	//最大耐久度の乱数範囲
	float randomRate;
};

struct CSVData {
	Vector3 size;
	std::string chunkDataDirectoryPath;
	std::vector<VoxelStatus> voxelStatus;
	std::string voxelDataFilePath;
};

class Voxel {
private:

	//ボクセルサイズ
	const float scale = 3.0f;

	std::shared_ptr<DirectionalLight> directionalLight_;

	//チャンク単位のボクセル
	std::vector<std::vector<std::vector<Chunk>>> chunks_;
	//描画する面
	std::shared_ptr<Model> face_;

	//描画可能なオブジェクト
	std::array<std::unique_ptr<Object>, 32768> objects_;
	//実際に描画するリスト
	std::list<Object*> drawOdjects_;
	uint32_t index_ = 0;

	Course* course_;

	GameCamera* camera_;

	//セーブするときに使う
	CSVData csvData_;

public:
	void Initialize(Course* course, std::shared_ptr<Model> face, CSVData csvData, GameCamera* camera, std::shared_ptr<DirectionalLight> directionalLigth);

	void Update();

	void Draw();
	void Draw(AABB drawRange);
	void DrawAll();
	void DrawUp();

	void Collision(Sphere sphere);

	CollisionVoxel GetCollisionVoxel(Ray ray, AABB chunkRate);

	// 衝突位置を返す
	std::optional<Vector3> CollisionCheck(Sphere sphere);

	std::vector<std::vector<std::vector<Chunk>>> GetChunks() { return chunks_; }

	//ボクセルのセット
	void SetVoxel(Vector3 chunkPos, int y, int z, int x, uint8_t voxelNum) { chunks_[int(chunkPos.y)][int(chunkPos.z)][int(chunkPos.x)].mapChip[y][z][x] = voxelNum; }
	//上のマップチップをコピー
	void CopyUpperMapChip(Vector3 chunkPos, int y);
	//下のマップチップをコピー
	void CopyUnderMapChip(Vector3 chunkPos, int y);
	//チャンク縦回転
	void ChunkVerticalRotation(Vector3 chunkPos);
	//チャンク横回転
	void ChunkHorizontalRotation(Vector3 chunkPos);
	//チャンクコピー
	void ChunkCopy(Vector3 fromChunkPos, Vector3 toChunkPos);
	//チャンク交換
	void ChunkSwap(Vector3 fromChunkPos, Vector3 toChunkPos);

	//チャンク数変更
	void Resize(Vector3 size);
	//上にチャンク入れ込み
	void AddChunkY(int chunkPos);

	void SetVoxelData(std::vector<VoxelStatus> voxelStatus) { csvData_.voxelStatus = voxelStatus; }
	std::vector<VoxelStatus>  GetVoxelData() { return csvData_.voxelStatus; }

	//セーブ
	void Save(const std::string& directoryPath);

	// 一定Yチャンク範囲内のブロック数
	int CountObjects(int startY, int endY);

private:

	void DrawChunk(int chunkY, int chunkZ, int chunkX, Vector3 cameraTranslate, Vector3 cameraChunkNumber);
	void DrawChunkAll(int chunkY, int chunkZ, int chunkX, Vector3 cameraChunkNumber);
	void DrawChunkAllUp(int chunkY, int chunkZ, int chunkX, Vector3 cameraChunkNumber, int maxUpRange);

	Object* AddFace(int chunkY, int chunkZ, int chunkX, int y, int z, int x, int8_t number, Quaternion rotate);

	Chunk LoadChunk(std::string loadFile);
	void WriteChunk(const Chunk& chunk, const std::string& loadFile);

	std::vector<VoxelStatus> LoadVoxel(std::string loadFile);
	void WriteVoxel(const std::string& loadFile);
};