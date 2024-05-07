struct PSInput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

Texture2D<float4> colorTex : register(t0);
Texture2D<float4> normalTex : register(t1);
Texture2D<float4> depthTex : register(t2);
SamplerState smp : register(s0);

struct PSOutput {
    float4 color : SV_TARGET0;
};

float32_t3 HSVToRGB(in float32_t3 hsv) {
	float32_t4 k = float32_t4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
	float32_t3 p = abs(frac(hsv.xxx + k.xyz) * 6.0f - k.www);
	return hsv.z * lerp(k.xxx, clamp(p - k.xxx, 0.0f, 1.0f), hsv.y);
}

PSOutput main(PSInput input) {
    PSOutput output;
	output.color = colorTex.Sample(smp, input.texcoord);
	output.color = output.color * output.color * output.color * output.color;
    return output;
}