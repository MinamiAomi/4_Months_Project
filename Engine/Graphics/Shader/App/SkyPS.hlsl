struct Scene {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 worldMatrix;
    float4x4 worldInverseTransposeMatrix;
    float3 topColor;
    float3 bottomColor;
};

ConstantBuffer<Scene> g_Scene : register(b0);

Texture2D<float4> g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct PSInput {
    float4 svPosition : SV_POSITION;
    float3 worldPosition : POSITION0;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float4 color : SV_TARGET0;
};

float3 HSVToRGB(in float3 hsv) {
    float4 k = float4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
    float3 p = abs(frac(hsv.xxx + k.xyz) * 6.0f - k.www);
    return hsv.z * lerp(k.xxx, clamp(p - k.xxx, 0.0f, 1.0f), hsv.y);
}

PSOutput main(PSInput input) {
    float3 normal = normalize(input.normal);
    
    PSOutput output;
    output.color.rgb = 0.0f;
    output.color.a = 1.0f;
    

    if (normal.y <= -0.9f) {
        output.color.rgb = HSVToRGB(g_Scene.topColor.xyz);
    }
    else if (normal.y >= 0.9f) {
        output.color.rgb = HSVToRGB(g_Scene.bottomColor.xyz);
    }
    else {
        float3 hsv = lerp(g_Scene.topColor, g_Scene.bottomColor, input.texcoord.y);
        output.color.rgb = HSVToRGB(hsv.xyz);
    }

    output.color.rgb += g_Texture.Sample(g_Sampler, input.texcoord * 5.0f).rgb;
    
    return output;
}