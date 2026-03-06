#pragma once
#include <string>
#include <array>
#include <list>
#include "DirectXCommon/DirectXCommon.h"
#include "SRVManager/SRVManager.h"

#include "ObjectVertexData.h"
#include "Vector2.h"
#include "Offset.h"

#include "Line.h"
#include "Plane.h"
#include "AABB.h"
#include "DirectionalLight/DirectionalLight.h"

using namespace std;

class OptionalPrimitiveManager {
private:

	static unique_ptr<OptionalPrimitiveManager> instance;

	DirectXCommon* dxCommon_ = nullptr;

	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//頂点リソースデータ
	ObjectVertexData* vertexData_ = nullptr;
	//インデックスリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//インデックスデータ
	uint32_t* indexData_ = nullptr;

	std::vector<ObjectVertexData> vertices_;
	std::vector<uint32_t>   indices_;
	std::shared_ptr<DirectionalLight> directionalLight_;

public:
	OptionalPrimitiveManager() = default;
	~OptionalPrimitiveManager() = default;
	OptionalPrimitiveManager(OptionalPrimitiveManager&) = delete;
	OptionalPrimitiveManager& operator=(OptionalPrimitiveManager&) = delete;

	static OptionalPrimitiveManager* GetInstance();

	//初期化
	void Initialize(DirectXCommon* dxCommon);

	void Finalize();

	void Draw();

	void Build(std::vector<ObjectVertexData> vertices, std::vector<uint32_t> indices);

	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView_; }
	D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return indexBufferView_; }
	UINT GetIndexCount() { return UINT(indices_.size()); }

	void SetDirectionalLight(const std::shared_ptr<DirectionalLight>& directionalLight) { directionalLight_ = directionalLight; }
};