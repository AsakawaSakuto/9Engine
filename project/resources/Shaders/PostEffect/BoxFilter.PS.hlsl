#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // テクスチャサイズを取得してテクセルサイズを計算
    int width, height;
    gTexture.GetDimensions(width, height);
    float2 texelSize = float2(1.0f / (float)width, 1.0f / (float)height);

    // 3x3 ボックスフィルタ（全サンプルの平均）
    float4 sum = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            float2 offset = float2((float)x, (float)y) * texelSize;
            sum += gTexture.Sample(gSampler, input.texcoord + offset);
        }
    }
    output.color = sum / 9.0f;

    return output;
}
