#pragma once
#include <string>
#include <array>
#include <list>
#include "DirectXCommon/DirectXCommon.h"
#include "SRVManager/SRVManager.h"

#include "VertexData.h"
#include "Vector2.h"
#include "Offset.h"

#include "Line.h"
#include "Plane.h"
#include "AABB.h"

using namespace std;

class PrimitiveManager {
public:
	//プリミティブで使用可能な形
	enum Primitive_SHAPE {
		SHAPE_Line,
		SHAPE_Plane,
		SHAPE_Point,
		SHAPE_AABB,
		SHAPE_count
	};
	//プリミティブのインスタシング用リソース
	struct PrimitiveResource {
		Offset offset;
		uint32_t instancingIndex = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
	};
	//描画可能なプリミティブの数
	static const uint32_t kMaxNumPrimitive = 1024;

	struct PrimitiveLine {
		Line line;
		Vector4 color;
	};
	struct PrimitivePoint {
		Vector3 point;
		Vector4 color;
	};
	struct PrimitivePlane {
		Plane plane;
		Vector4 color;
	};
	struct PrimitiveAABB {
		AABB aabb;
		Vector4 color;
	};
private:

	static unique_ptr<PrimitiveManager> instance;

	DirectXCommon* dxCommon_ = nullptr;

	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//頂点リソースデータ
	VertexData* vertexData_ = nullptr;
	//インデックスリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//インデックスデータ
	uint32_t* indexData_ = nullptr;

	//インスタシング用リソース
	std::array<PrimitiveResource, SHAPE_count> primitiveResource_;
	//直線データ
	std::array<PrimitiveLine, kMaxNumPrimitive> line_;
	int32_t lineIndex_ = 0;
	//平面データ
	std::array<PrimitivePlane, kMaxNumPrimitive> plane_;
	int32_t planeIndex_ = 0;
	//点データ
	std::array<PrimitivePoint, kMaxNumPrimitive> point_;
	int32_t pointIndex_ = 0;
	//AABBデータ
	std::array<PrimitiveAABB, kMaxNumPrimitive> aabb_;
	int32_t aabbIndex_ = 0;

public:

	PrimitiveManager() = default;
	~PrimitiveManager() = default;
	PrimitiveManager(PrimitiveManager&) = delete;
	PrimitiveManager& operator=(PrimitiveManager&) = delete;

	static PrimitiveManager* GetInstance();

	//初期化
	void Initialize(DirectXCommon* dxCommon, SRVManager* srvManager);

	void Finalize();

	void Draw();

	//直線
	void AddLine(Line line, Vector4 color = { 1,0,0,1 }) { line_[lineIndex_].line = line; line_[lineIndex_].color = color; lineIndex_++; };
	//半直線
	void AddRay(Ray ray, Vector4 color = { 1,0,0,1 }) { Line line = { .origin = ray.origin,.diff = ray.diff }; AddLine(line, color); };
	//線分
	void AddSegment(Segment segment, Vector4 color = { 1,0,0,1 }) { Line line = { .origin = segment.origin,.diff = segment.diff }; AddLine(line, color); };
	//平面
	void AddPlane(Plane plane, Vector4 color = { 1,0,0,1 }) { plane_[planeIndex_].plane = plane; plane_[planeIndex_].color = color; planeIndex_++; };
	//AABB
	void AddPoint(Vector3 point, Vector4 color = { 1,0,0,1 }) { point_[pointIndex_].point = point; point_[pointIndex_].color = color; pointIndex_++; };
	//AABB
	void AddAABB(AABB aabb, Vector4 color = { 1,0,0,1 }) { aabb_[aabbIndex_].aabb = aabb; aabb_[aabbIndex_].color = color; aabbIndex_++; };

	//リセット
	void Reset();

	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView_; }
	D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return indexBufferView_; }
};