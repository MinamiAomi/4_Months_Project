struct Param {
    float3 topColor;
    float3 bottomColor;
};

ConstantBuffer<Param> g_Param : register(b2);

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

    PSOutput output;
    output.color.rgb = 0.0f;
    output.color.a = 1.0f;
    

    if (input.normal.y <= -0.9f) {
        output.color.rgb = HSVToRGB(g_Param.topColor.xyz);
    }
    else if (input.normal.y >= 0.9f) {
        output.color.rgb = HSVToRGB(g_Param.bottomColor.xyz);
    }
    else {
        float3 hsv = lerp(g_Param.topColor, g_Param.bottomColor, input.texcoord.y);

        output.color.rgb = HSVToRGB(hsv.xyz);
    }

    output.color.rgb += g_Texture.Sample(g_Sampler, input.texcoord).rgb;
    
    return output;
}