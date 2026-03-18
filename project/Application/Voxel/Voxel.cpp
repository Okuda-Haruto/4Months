#include "Voxel.h"
#include "Scene/BaseScene/GameScene/GameScene.h"
#include <Collision.h>

#include <numbers>
#include <cassert>

void Voxel::Initialize(GameScene* gameScene ,std::shared_ptr<Model> face_, std::shared_ptr<DirectionalLight> directionalLigth) {

	gameScene_ = gameScene;
	directionalLigth_ = directionalLigth;

	for (index_ = 0; index_ < objects_.size(); index_++) {
		objects_[index_] = std::make_unique<Object>();
		objects_[index_]->Initialize(face_);
		objects_[index_]->SetShininess(0);
	}

	index_ = 0;

	Chunk chunk;
	chunk = LoadChunk("resources/CSV/chunk_01.csv");
	chunks_.push_back(chunk);
	chunk = LoadChunk("resources/CSV/chunk_02.csv");
	chunks_.push_back(chunk);
	chunk = LoadChunk("resources/CSV/chunk_03.csv");
	chunks_.push_back(chunk);
	chunk = LoadChunk("resources/CSV/chunk_04.csv");
	chunks_.push_back(chunk);
}

void Voxel::Update() {

}

void Voxel::Draw() {
	std::list<Object*> objects;
	for (int i = 0; i < 4; i++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				for (int x = 0; x < 16; x++) {
					if (!chunks_[i].mapChip[y][z][x])continue;
					//上面
					if (y == 0) {
						//上が空白
						if (i == 0) {
							if (index_ < objects_.size()) {
								objects.push_back(AddFace(i, y, z, x, MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
								index_++;
							}
						} else {
							if (!chunks_[i - 1].mapChip[15][z][x]) {
								if (index_ < objects_.size()) {
									objects.push_back(AddFace(i, y, z, x, MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
									index_++;
								}
							}
						}
					} else {
						if (!chunks_[i].mapChip[y - 1][z][x]) {
							if (index_ < objects_.size()) {
								objects.push_back(AddFace(i, y, z, x, MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
								index_++;
							}
						}
					}
					//前面
					if (z != 0) {
						if (!chunks_[i].mapChip[y][z - 1][x]) {
							if (index_ < objects_.size()) {
								objects.push_back(AddFace(i, y, z, x, MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>)));
								index_++;
							}
						}
					}
					//後面
					if (z != 15) {
						if (!chunks_[i].mapChip[y][z + 1][x]) {
							if (index_ < objects_.size()) {
								objects.push_back(AddFace(i, y, z, x, IdentityQuaternion()));
								index_++;
							}
						}
					}
					//右面
					if (x != 15) {
						if (!chunks_[i].mapChip[y][z][x + 1]) {
							if (index_ < objects_.size()) {
								objects.push_back(AddFace(i, y, z, x, MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 2)));
								index_++;
							}
						}
					}
					//左面
					if (x != 0) {
						if (!chunks_[i].mapChip[y][z][x - 1]) {
							if (index_ < objects_.size()) {
								objects.push_back(AddFace(i, y, z, x, MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> + std::numbers::pi_v<float> / 2)));
								index_++;
							}
						}
					}
				}
			}
		}
	}

	GameEngine::DrawInstancingObject_3D(objects, directionalLigth_, nullptr, nullptr);

	index_ = 0;
}

Object* Voxel::AddFace(int i, int y, int z, int x, Quaternion rotate) {

	if (index_ < objects_.size()) {
		SRT transform;
		transform.scale = { scale,scale,scale };
		transform.rotate = rotate;
		transform.translate = {
			(x * scale * 2 - 16 * scale - scale / 2),
			-(y * scale * 2) - scale / 2 - i * 32 * scale ,
			32 * scale - (z * scale * 2 + 16 * scale) - scale / 2,
		};

		objects_[index_]->SetTransform(transform);

		return objects_[index_].get();
	}

	assert(false);

	return nullptr;
}

void Voxel::Collision(Sphere sphere) {
	for (int i = 0; i < 4; i++) {
		AABB chunkAABB;
		chunkAABB.min = {
			-16 * scale - scale - scale / 2 ,
			-(i + 1) * 32 * scale + scale / 2,
			-16 * scale + scale / 2,
		};
		chunkAABB.max = chunkAABB.min + Vector3{32 * scale,32 * scale ,32 * scale };

		if (IsCollision(chunkAABB, sphere)) {
			for (int y = 0; y < 16; y++) {
				if (sphere.center.y + sphere.radius >= -(y * scale * 2) - scale - i * 32 * scale - scale &&
					sphere.center.y - sphere.radius <= -(y * scale * 2) - scale - i * 32 * scale) {
					for (int z = 0; z < 16; z++) {
						if (sphere.center.z + sphere.radius >= 32 * scale - (z * scale * 2 + 16 * scale) - scale &&
							sphere.center.z - sphere.radius <= 32 * scale - (z * scale * 2 + 16 * scale)) {
							for (int x = 0; x < 16; x++) {
								if (!chunks_[i].mapChip[y][z][x])continue;

								AABB voxelAABB;
								voxelAABB.min = {
									(x * scale * 2 - 16 * scale - scale - scale / 2) - -scale / 2,
									-(y * scale * 2) - scale - i * 32 * scale - scale ,
									32 * scale - (z * scale * 2 + 16 * scale) - scale,
								};
								voxelAABB.max = voxelAABB.min + Vector3{ scale * 2,scale * 2 ,scale * 2 };

								if (IsCollision(voxelAABB, sphere)) {
									chunks_[i].mapChip[y][z][x] = 0;
									SRT transform;
									transform.scale = { scale,scale,scale };
									transform.rotate = IdentityQuaternion();
									transform.translate = {
										(x * scale * 2 - 16 * scale - scale / 2),
										-(y * scale * 2) - scale / 2 - i * 32 * scale ,
										32 * scale - (z * scale * 2 + 16 * scale) - scale / 2,
									};
									gameScene_->AddEffect(transform);
								}
							}
						}
					}
				}
			}
		}
	}


}

Chunk Voxel::LoadChunk(std::string loadFile) {
	std::ifstream file(loadFile);
	assert(file.is_open());

	Chunk chunk{};

	std::string line;

	int y = 0; // 高さ（層）
	int z = 0; // 奥行き

	while (std::getline(file, line)) {

		// 空行 → 次の層へ
		if (line.empty()) {
			y++;
			z = 0;
			continue;
		}

		// コメント
		if (line.rfind("//", 0) == 0) {
			continue;
		}

		std::stringstream ss(line);
		std::string cell;

		int x = 0; // 横

		// カンマ区切りで読む
		while (std::getline(ss, cell, ',')) {
			chunk.mapChip[y][z][x] = std::stoi(cell);
			x++;
		}

		z++;
	}

	file.close();
	return chunk;
}