#include "3d/Effect/Effect.h"
#include "imgui.h"
#include <cassert>
#include <algorithm>
#include <numbers>

void Effect::Initialize() {
	assert(!isInitialized_ && "Effect::Initialize() を2回呼ばないでください。");

	// --- Planeモデルの初期化 ---
	if (params_.usePlane) {
		planeModel_ = std::make_unique<Model>();
		// "plane" という名前のモデルをロード（resources/model/plane/plane.obj 等）
		planeModel_->Initialize("plane.obj");
		planeModel_->SetTransparent(true);

		// テクスチャとブレンドモードを適用
		planeModel_->SetTexture(params_.plane.texturePath);

		// Planeインスタンスのバッファを確保（最大spawnCount個）
		planeInstances_.resize(params_.plane.spawnCount);
	}

	// --- Particleシステムの初期化 ---
	if (params_.useParticle) {
		particles_ = std::make_unique<Particles>();
		particles_->Initialize(params_.particleJsonPath);
		particles_->SetBlendMode(kBlendModeAdd);
		particles_->SetOffSet(params_.particleOffset);
	}

	isInitialized_ = true;
}

void Effect::Play(const Vector3& position) {
	assert(isInitialized_ && "Play() の前に Initialize() を呼んでください。");

	// Planeを生成
	if (params_.usePlane) {
		SpawnPlanes(position);
	}

	// Particleを一回だけ発射
	if (params_.useParticle && particles_) {
		particles_->Play(position, false);
	}

	isPlaying_ = true;
}

void Effect::Stop() {
	if (params_.usePlane) {
		for (auto& inst : planeInstances_) {
			inst.alive = false;
		}
	}
	if (params_.useParticle && particles_) {
		particles_->Stop();
	}
	isPlaying_ = false;
}

void Effect::Update(float deltaTime) {
	if (!isInitialized_) return;

	if (params_.usePlane) {
		UpdatePlanes(deltaTime);
	}

	if (params_.useParticle && particles_) {
		particles_->Update(deltaTime);
	}

	// 全Planeが死亡 & Particle停止 → 再生終了
	if (isPlaying_) {
		bool anyAlive = false;
		for (const auto& inst : planeInstances_) {
			if (inst.alive) { anyAlive = true; break; }
		}
		if (params_.useParticle && particles_ && particles_->IsPlaying()) {
			anyAlive = true;
		}
		if (!anyAlive) {
			isPlaying_ = false;
		}
	}
}

void Effect::Draw(Camera& camera) {
	if (!isInitialized_) return;

	if (params_.usePlane) {
		DrawPlanes(camera);
	}

	if (params_.useParticle && particles_) {
		particles_->Draw(camera);
	}
}

void Effect::DrawImGui(const char* label) {
#ifdef USE_IMGUI
	ImGui::Begin(label);

	ImGui::Text("IsPlaying: %s", isPlaying_ ? "true" : "false");
	ImGui::Separator();

	// --- Plane設定 ---
	if (ImGui::CollapsingHeader("Plane Settings")) {
		ImGui::Checkbox("Use Plane",    &params_.usePlane);

		auto& p = params_.plane;
		ImGui::DragFloat("ScaleX",      &p.scaleX,    0.001f, 0.001f, 10.0f);
		ImGui::DragFloat("ScaleYMin",   &p.scaleYMin, 0.01f,  0.001f, 10.0f);
		ImGui::DragFloat("ScaleYMax",   &p.scaleYMax, 0.01f,  0.001f, 10.0f);
		ImGui::DragFloat("RotateZMin",  &p.rotateZMin,0.01f, -(float)std::numbers::pi, 0.0f);
		ImGui::DragFloat("RotateZMax",  &p.rotateZMax,0.01f,  0.0f, (float)std::numbers::pi);
		ImGui::DragFloat("LifeTime",    &p.lifeTime,  0.01f,  0.01f, 10.0f);
		ImGui::DragInt("SpawnCount",    reinterpret_cast<int*>(&p.spawnCount), 1, 1, 64);
		ImGui::ColorEdit4("StartColor", &p.startColor.x);
		ImGui::ColorEdit4("EndColor",   &p.endColor.x);
	}

	// --- Particle設定 ---
	if (ImGui::CollapsingHeader("Particle Settings")) {
		ImGui::Checkbox("Use Particle", &params_.useParticle);
		if (params_.useParticle && particles_) {
			particles_->DrawImGui("HitEffect_Particle");
		}
	}

	// --- テスト再生 ---
	if (ImGui::Button("Play at Origin")) {
		Play({ 0.0f, 0.0f, 0.0f });
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop")) {
		Stop();
	}

	ImGui::End();
#endif
}

void Effect::SpawnPlanes(const Vector3& position) {
	const auto& p = params_.plane;

	// ランダム分布の準備
	std::uniform_real_distribution<float> distScaleY(p.scaleYMin, p.scaleYMax);
	std::uniform_real_distribution<float> distRotZ  (p.rotateZMin, p.rotateZMax);
	std::uniform_real_distribution<float> distVelX  (p.velocityMin.x, p.velocityMax.x);
	std::uniform_real_distribution<float> distVelY  (p.velocityMin.y, p.velocityMax.y);
	std::uniform_real_distribution<float> distVelZ  (p.velocityMin.z, p.velocityMax.z);

	// インスタンス配列をリサイズ（spawnCountが変化した場合に対応）
	planeInstances_.resize(p.spawnCount);

	for (uint32_t i = 0; i < p.spawnCount; ++i) {
		PlaneInstance& inst = planeInstances_[i];

		// --- Transform ---
		inst.position   = position;
		inst.scale      = { p.scaleX, distScaleY(randomEngine_), p.scaleZ };
		inst.rotate     = { 0.0f, 0.0f, distRotZ(randomEngine_) };
		inst.velocity   = { distVelX(randomEngine_),
							distVelY(randomEngine_),
							distVelZ(randomEngine_) };

		// --- 見た目 ---
		inst.startColor = p.startColor;
		inst.endColor   = p.endColor;
		inst.color      = p.startColor;

		// --- ライフタイム ---
		inst.lifeTime    = p.lifeTime;
		inst.currentTime = 0.0f;
		inst.alive       = true;
	}
}

void Effect::UpdatePlanes(float deltaTime) {
	for (auto& inst : planeInstances_) {
		if (!inst.alive) continue;

		inst.currentTime += deltaTime;

		if (inst.currentTime >= inst.lifeTime) {
			inst.alive = false;
			continue;
		}

		// 正規化ライフ比率（0.0 → 1.0）
		const float t = inst.currentTime / inst.lifeTime;

		// 位置を速度で更新
		inst.position.x += inst.velocity.x * deltaTime;
		inst.position.y += inst.velocity.y * deltaTime;
		inst.position.z += inst.velocity.z * deltaTime;

		// 色をスタート→エンドへ線形補間（フェードアウト等）
		inst.color = LerpColor(inst.startColor, inst.endColor, t);
	}
}

void Effect::DrawPlanes(Camera& camera) {
	if (!planeModel_) return;

	for (const auto& inst : planeInstances_) {
		if (!inst.alive) continue;

		Transform transform;
		transform.scale     = inst.scale;
		transform.rotate    = inst.rotate;
		transform.translate = inst.position;

		planeModel_->SetColor4(inst.color);
		planeModel_->Draw(camera, transform);
	}
}

Vector4 Effect::LerpColor(const Vector4& a, const Vector4& b, float t) {
	return {
		Lerp(a.x, b.x, t),
		Lerp(a.y, b.y, t),
		Lerp(a.z, b.z, t),
		Lerp(a.w, b.w, t),
	};
}
