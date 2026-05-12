#include "SkyBox.hlsli"

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;

    // テクスチャ座標は入力位置をそのまま使用（中心からの方向ベクトル）
    output.texcoord = input.position.xyz;

    // 位置をクリップ空間に変換し、深度を最遠面に設定
    output.position = mul(input.position, gTransformationMatrix.WVP).xyww;

    return output;
}
