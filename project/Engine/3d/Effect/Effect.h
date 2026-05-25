#pragma once
#include <vector>
#include <memory>
#include <random>
#include <numbers>
#include <string>

#include "3d/Effect/EffectTypes.h"
#include "3d/Model/Model.h"
#include "3d/Particle/Particles.h"
#include "Camera/Camera.h"
#include "Math/Type/Vector3.h"

/// <summary>
/// ゲーム用エフェクトシステム
/// </summary>
class Effect {
public:
	Effect()  = default;
	~Effect() = default;

	// コピー禁止・ムーブ可
	Effect(const Effect&)            = delete;
	Effect& operator=(const Effect&) = delete;
	Effect(Effect&&)                 = default;
	Effect& operator=(Effect&&)      = default;

	/// <summary>
	/// 初期化。EffectParamsを設定してから呼ぶこと
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update(float deltaTime);

	/// <summary>
	/// 描画（不透明→半透明の描画順で呼ぶこと）
	/// </summary>
	void Draw(Camera& camera);

	/// <summary>
	/// ImGuiでパラメータをリアルタイム編集（デバッグ用）
	/// </summary>
	void DrawImGui(const char* label = "Effect");

	/// <summary>
	/// エフェクトを指定座標で再生（ワンショット）
	/// </summary>
	void Play(const Vector3& position);

	/// <summary>
	/// 再生停止・リセット
	/// </summary>
	void Stop();

	/// <summary>
	/// エフェクトパラメータを設定（Initialize前に呼ぶこと推奨）
	/// </summary>
	void SetParams(const EffectParams& params) { params_ = params; }

	/// <summary>
	/// 現在のパラメータを取得
	/// </summary>
	const EffectParams& GetParams() const { return params_; }

	/// <summary>
	/// エフェクトが再生中かどうか
	/// </summary>
	bool IsPlaying() const { return isPlaying_; }

private:
	// --- 内部処理 ---
	void SpawnPlanes(const Vector3& position);
	void UpdatePlanes(float deltaTime);
	void DrawPlanes(Camera& camera);

	float Lerp(float a, float b, float t) { return a + (b - a) * t; }
	Vector4 LerpColor(const Vector4& a, const Vector4& b, float t);

	// --- パラメータ ---
	EffectParams params_;

	// --- Planeインスタンス群 ---
	// 共有ModelリソースをPlane枚数分のTransformで描画する
	std::unique_ptr<Model>              planeModel_;
	std::vector<PlaneInstance>          planeInstances_;

	// --- Particleシステム ---
	std::unique_ptr<Particles>          particles_;

	// --- 状態 ---
	bool isPlaying_     = false;
	bool isInitialized_ = false;

	// --- 乱数生成器 ---
	std::mt19937 randomEngine_{ std::random_device{}() };
};
