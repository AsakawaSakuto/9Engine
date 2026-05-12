#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Math/Type/Transform.h"
#include "Math/Type/Matrix4x4.h"
#include "Math/Type/Vector3.h"
#include "Math/Type/Vector4.h"
#include "Camera/Camera.h"
#include "Core/DirectXCommon/DirectXCommon.h"

class SkyBox {
public:
	void Initialize(const std::string& texturePath);
	void Draw(Camera& camera);

	void SetTexture(const std::string& textureName);
	void SetColor(const Vector4& color) { materialData_->color = color; }
	void SetScale(float scale) { transform_.scale = { scale, scale, scale }; }

private:
	void CreateVertexResource();
	void CreateIndexResource();
	void CreateMaterialResource();
	void CreateTransformationResource();
	void UpdateMatrix(Camera& camera);

private:
	DirectXCommon* dxCommon_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_;

	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};

	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	Transform transform_;
	std::string textureName_;
	uint32_t textureIndex_ = 0;
};
