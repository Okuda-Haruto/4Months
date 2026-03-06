#include "OptionalPrimitiveManager.h"
#include <GameEngine.h>
#include <LoadObjFile.h>
#include <SpriteManager/SpriteManager.h>

unique_ptr<OptionalPrimitiveManager> OptionalPrimitiveManager::instance;

OptionalPrimitiveManager* OptionalPrimitiveManager::GetInstance() {
	if (!instance) {
		instance = make_unique<OptionalPrimitiveManager>();
	}
	return instance.get();
}

void OptionalPrimitiveManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
}

void OptionalPrimitiveManager::Finalize() {
	instance.reset();
}

void OptionalPrimitiveManager::Draw() {
	GameEngine::DrawOptionalPrimitive();
}

void OptionalPrimitiveManager::Build(std::vector<ObjectVertexData> vertices, std::vector<uint32_t> indices) {
	vertices_ = vertices;
	indices_ = indices;

	vertexResource_ =
		dxCommon_->CreateBufferResources(
			sizeof(ObjectVertexData) * vertices_.size());

	ObjectVertexData* mapped = nullptr;

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapped));

	memcpy(mapped, vertices_.data(), sizeof(ObjectVertexData) * UINT(vertices_.size()));

	vertexResource_->Unmap(0, nullptr);

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(ObjectVertexData) * UINT(vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(ObjectVertexData);

	indexResource_ = dxCommon_->CreateBufferResources(sizeof(uint32_t) * indices_.size());

	uint32_t* mappedIndex;

	indexResource_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>(&mappedIndex));

	memcpy(mappedIndex, indices_.data(), sizeof(uint32_t) * indices_.size());

	indexResource_->Unmap(0, nullptr);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * UINT(indices_.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}