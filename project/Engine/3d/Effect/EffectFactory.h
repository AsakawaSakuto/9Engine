#pragma once
#include "3d/Effect/Effect.h"
#include <numbers>

/// <summary>
/// よく使うエフェクトのプリセットを生成するファクトリ
/// </summary>
namespace EffectFactory {

	/// <summary>
	/// 打撃エフェクト（星型スパーク）
	///   - 縦長Planeを8枚ランダムZ回転で放射状に配置
	///   - 加算合成で鋭い閃光を表現
	/// </summary>
	inline std::unique_ptr<Effect> CreateHitEffect() {
		EffectParams params;
		params.effectName    = "HitEffect";
		params.usePlane      = true;
		params.useParticle   = true;

		// --- Plane設定 ---
		auto& p = params.plane;
		p.scaleX        = 0.05f;                          // X方向を細く（縦長ライン）
		p.scaleYMin     = 0.3f;                           // Y方向ランダム下限
		p.scaleYMax     = 0.8f;                           // Y方向ランダム上限
		p.scaleZ        = 1.0f;
		p.rotateZMin    = -(float)std::numbers::pi;       // Z回転 -π
		p.rotateZMax    =  (float)std::numbers::pi;       // Z回転 +π
		p.lifeTime      = 0.4f;
		p.spawnCount    = 8;                              // 8枚を星型配置
		p.startColor    = { 1.0f, 0.9f, 0.4f, 1.0f };    // 黄白色
		p.endColor      = { 1.0f, 0.5f, 0.0f, 0.0f };    // オレンジ→透明
		p.texturePath   = "resources/image/particle/circle2.png";
		p.blendMode     = kBlendModeAdd;

		// --- Particle設定（JSON読み込み） ---
		params.particleJsonPath = "hitEffect";
		params.particleOffset   = { 0.0f, 0.0f, 0.0f };

		auto effect = std::make_unique<Effect>();
		effect->SetParams(params);
		return effect;
	}

	/// <summary>
	/// 剣撃エフェクト
	///   - より細く長いPlaneを4枚、高速フェードアウト
	///   - 加算合成で白い斬撃ラインを表現
	/// </summary>
	inline std::unique_ptr<Effect> CreateSlashEffect() {
		EffectParams params;
		params.effectName    = "SlashEffect";
		params.usePlane      = true;
		params.useParticle   = false;

		auto& p = params.plane;
		p.scaleX        = 0.03f;
		p.scaleYMin     = 0.6f;
		p.scaleYMax     = 1.2f;
		p.scaleZ        = 1.0f;
		p.rotateZMin    = -(float)std::numbers::pi;
		p.rotateZMax    =  (float)std::numbers::pi;
		p.lifeTime      = 0.25f;
		p.spawnCount    = 4;
		p.startColor    = { 1.0f, 1.0f, 1.0f, 1.0f };
		p.endColor      = { 0.6f, 0.8f, 1.0f, 0.0f };
		p.texturePath   = "resources/image/particle/circle2.png";
		p.blendMode     = kBlendModeAdd;

		auto effect = std::make_unique<Effect>();
		effect->SetParams(params);
		return effect;
	}

} // namespace EffectFactory
