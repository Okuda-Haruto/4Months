#include "Voxel.h"

#include <numbers>
#include <cassert>

void Voxel::Initialize(std::shared_ptr<Model> face_, std::shared_ptr<DirectionalLight> directionalLigth) {

	directionalLigth_ = directionalLigth;

	for (index_ = 0; index_ < objects_.size(); index_++) {
		objects_[index_] = std::make_unique<Object>();
		objects_[index_]->Initialize(face_);
	}

	index_ = 0;

	Chunk chunk;
	for (int y = 0; y < 16; y++) {
		for (int z = 0; z < 16; z++) {
			for (int x = 0; x < 16; x++) {
				if (x <= 0 || x >= 15 || z <= 0 || z >= 15) {
					chunk.mapChip[y][z][x] = 1;
				} else {
					chunk.mapChip[y][z][x] = 0;
				}

				if (x >= 7 && x <= 9 && y >= 5 && y <= 8 && z >= 7 && z <= 9) {
					chunk.mapChip[y][z][x] = 1;
				}

				if (x >= 5 && x <= 9 && y >= 7 && y <= 8 && z >= 7 && z <= 9) {
					chunk.mapChip[y][z][x] = 1;
				}
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		chunks_.push_back(chunk);
	}
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
							if (index_ <  objects_.size() - 1) {
								objects.push_back(AddFace(i, y, z, x, MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
								index_++;
							}
						} else {
							if (!chunks_[i - 1].mapChip[15][z][x]) {
								if (index_ <  objects_.size() - 1) {
									objects.push_back(AddFace(i, y, z, x, MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
									index_++;
								}
							}
						}
					} else {
						if (!chunks_[i].mapChip[y - 1][z][x]) {
							if (index_ < objects_.size() - 1) {
								objects.push_back(AddFace(i, y, z, x, MakeRotateAxisAngleQuaternion(Vector3{ 1,0,0 }, -std::numbers::pi_v<float> / 2)));
								index_++;
							}
						}
					}
					//前面
					if (z != 0) {
						if (!chunks_[i].mapChip[y][z - 1][x]) {
							if (index_ < objects_.size() - 1) {
								objects.push_back(AddFace(i, y, z, x, MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float>)));
								index_++;
							}
						}
					}
					//後面
					if (z != 15) {
						if (!chunks_[i].mapChip[y][z + 1][x]) {
							if (index_ <  objects_.size() - 1) {
								objects.push_back(AddFace(i, y, z, x, IdentityQuaternion()));
								index_++;
							}
						}
					}
					//右面
					if (x != 15) {
						if (!chunks_[i].mapChip[y][z][x + 1]) {
							if (index_ <  objects_.size() - 1) {
								objects.push_back(AddFace(i, y, z, x, MakeRotateAxisAngleQuaternion(Vector3{ 0,1,0 }, std::numbers::pi_v<float> / 2)));
								index_++;
							}
						}
					}
					//左面
					if (x != 0) {
						if (!chunks_[i].mapChip[y][z][x - 1]) {
							if (index_ <  objects_.size() - 1) {
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

	if (index_ <  objects_.size() - 1) {
		static float scale = 3.0f;
		SRT transform;
		transform.scale = { scale,scale,scale };
		transform.rotate = rotate;
		transform.translate = {
			(x * scale * 2 - 16 * scale - scale / 2),
			32 * scale - (y * scale * 2) - scale / 2 - (i + 1) * 32 * scale ,
			32 * scale - (z * scale * 2 + 16 * scale) - scale / 2,
		};

		if (index_ == 3643) {
			index_ = 3643;
		}

		objects_[index_]->SetTransform(transform);

		return objects_[index_].get();
	}

	assert(false);

	return nullptr;
}