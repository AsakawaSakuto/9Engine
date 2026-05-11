struct VertexShaderInput {
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VertexShaderOutput {
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD0;
};

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

struct TransformationMatrix {
    float4x4 WVP;
    float4x4 World;
};

struct Material {
    float4 color;
    int enableLighting;
    float3 padding1;
    float4x4 uvTransform;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<Material> gMaterial : register(b1);
TextureCube<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
