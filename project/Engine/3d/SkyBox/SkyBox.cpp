#include "SkyBox.h"
#include "Core/ServiceLocator/ServiceLocator.h"
#include "Core/TextureManager/TextureManager.h"
#include "Core/PSOManager/PSOManager.h"
#include "Core/CreateResource/CreateResource.h"
#include "Math/MatrixFunction/MatrixFunction.h"
#include <cassert>

void SkyBox::Initialize(const std::string& texturePath) {
	dxCommon_ = ServiceLocator::GetDXCommon();
	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();

	transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	CreateVertexResource();
	CreateIndexResource();
	CreateMaterialResource();
	CreateTransformationResource();

	SetTexture(texturePath);

	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentityMatrix();
}

void SkyBox::Draw(Camera& camera) {
	UpdateMatrix(camera);

	// コマンドリストを毎回取得
	commandList_ = dxCommon_->GetCommandList();

	auto pso = PSOManager::GetInstance().GetPSO(PSOType::SkyBox_Normal);
	auto rootSignature = PSOManager::GetInstance().GetRootSignature("SkyBox");

	commandList_->SetGraphicsRootSignature(rootSignature.Get());
	commandList_->SetPipelineState(pso.Get());

	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(&indexBufferView_);
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->SetGraphicsRootConstantBufferView(0, transformationResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));

	commandList_->DrawIndexedInstanced(36, 1, 0, 0, 0);
}

void SkyBox::SetTexture(const std::string& textureName) {
	textureName_ = textureName;
	TextureManager::GetInstance()->LoadTexture(textureName);
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName);
}

void SkyBox::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * 24);
	assert(vertexResource_ != nullptr && "Failed to create vertex resource");

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 24;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	assert(vertexBufferView_.BufferLocation != 0 && "Vertex buffer GPU address is null");

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	assert(vertexData_ != nullptr && "Failed to map vertex data");

	float size = 1.0f;

	VertexData vertices[24] = {
		// Front face (Z+)
		{{-size, -size,  size, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ size, -size,  size, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ size,  size,  size, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{-size,  size,  size, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		// Back face (Z-)
		{{ size, -size, -size, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
		{{-size, -size, -size, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
		{{-size,  size, -size, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{ size,  size, -size, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		// Left face (X-)
		{{-size, -size, -size, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-size, -size,  size, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-size,  size,  size, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-size,  size, -size, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
		// Right face (X+)
		{{ size, -size,  size, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		{{ size, -size, -size, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		{{ size,  size, -size, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{ size,  size,  size, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		// Top face (Y+)
		{{-size,  size,  size, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{ size,  size,  size, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{ size,  size, -size, 1.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{-size,  size, -size, 1.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		// Bottom face (Y-)
		{{-size, -size, -size, 1.0f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		{{ size, -size, -size, 1.0f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		{{ size, -size,  size, 1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{-size, -size,  size, 1.0f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}
	};

	std::memcpy(vertexData_, vertices, sizeof(vertices));
}

void SkyBox::CreateIndexResource() {
	indexResource_ = CreateBufferResource(device_.Get(), sizeof(uint32_t) * 36);
	assert(indexResource_ != nullptr && "Failed to create index resource");

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 36;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	assert(indexBufferView_.BufferLocation != 0 && "Index buffer GPU address is null");

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	assert(indexData_ != nullptr && "Failed to map index data");

	uint32_t indices[36] = {
		0, 1, 2, 2, 3, 0,       // Front
		4, 5, 6, 6, 7, 4,       // Back
		8, 9, 10, 10, 11, 8,    // Left
		12, 13, 14, 14, 15, 12, // Right
		16, 17, 18, 18, 19, 16, // Top
		20, 21, 22, 22, 23, 20  // Bottom
	};

	std::memcpy(indexData_, indices, sizeof(indices));
}

void SkyBox::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void SkyBox::CreateTransformationResource() {
	transformationResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
}

void SkyBox::UpdateMatrix(Camera& camera) {
	// SkyBoxはカメラの回転のみを考慮し、位置は無視する
	Matrix4x4 viewMatrix = camera.GetViewMatrix();

	// ビュー行列から平行移動成分を除去（回転のみを使用）
	Matrix4x4 viewRotationOnly = viewMatrix;
	viewRotationOnly.m[3][0] = 0.0f;
	viewRotationOnly.m[3][1] = 0.0f;
	viewRotationOnly.m[3][2] = 0.0f;

	// スケールのみを適用したワールド行列
	Matrix4x4 worldMatrix = MakeScaleMatrix(transform_.scale);

	// ビュー回転 * プロジェクション
	Matrix4x4 viewProjectionMatrix = MultiplyMatrix(viewRotationOnly, camera.GetProjectionMatrix());

	// ワールド * ビュー回転 * プロジェクション
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix(worldMatrix, viewProjectionMatrix);

	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;
}
