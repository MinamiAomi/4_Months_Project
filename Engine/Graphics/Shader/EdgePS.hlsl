struct Param {
    float32_t3 edgeColor;
};
ConstantBuffer<Param> param_  : register(b0);

struct PSInput {
    float4 position : SV_POSITION;
    float2 texCenter : TEXCOORD0;
    float2 texLT : TEXCOORD1;
    float2 texMT : TEXCOORD2;
    float2 texRT : TEXCOORD3;
    float2 texLM : TEXCOORD4;
    float2 texRM : TEXCOORD5;
    float2 texLB : TEXCOORD6;
    float2 texMB : TEXCOORD7;
    float2 texRB : TEXCOORD8;
};

Texture2D<float4> colorTex : register(t0);
Texture2D<float4> normalTex : register(t1);
Texture2D<float4> depthTex : register(t2);

SamplerState smp : register(s0);

struct PSOutput {
    float4 color : SV_TARGET0;
};

PSOutput main(PSInput input) {
    PSOutput output;

	float3 Normal;
	//周囲のピクセルの法線の値の平均を計算する
	Normal = normalTex.Sample(smp, input.texCenter).xyz * -8.0f;
	Normal += normalTex.Sample(smp, input.texLT.xy).xyz;
	Normal += normalTex.Sample(smp, input.texMT.xy).xyz;
	Normal += normalTex.Sample(smp, input.texRT.xy).xyz;
	Normal += normalTex.Sample(smp, input.texLM.xy).xyz;
	Normal += normalTex.Sample(smp, input.texRM.xy).xyz;
	Normal += normalTex.Sample(smp, input.texLB.xy).xyz;
	Normal += normalTex.Sample(smp, input.texMB.xy).xyz;
	Normal += normalTex.Sample(smp, input.texRB.xy).xyz;

	//周囲のピクセルの深度値の平均を計算する。
	float depth = depthTex.Sample(smp, input.texCenter.xy).x * -8.0f;
	depth += depthTex.Sample(smp, input.texLT.xy).x;
	depth += depthTex.Sample(smp, input.texMT.xy).x;
	depth += depthTex.Sample(smp, input.texRT.xy).x;
	depth += depthTex.Sample(smp, input.texLM.xy).x;
	depth += depthTex.Sample(smp, input.texRM.xy).x;
	depth += depthTex.Sample(smp, input.texLB.xy).x;
	depth += depthTex.Sample(smp, input.texMB.xy).x;
	depth += depthTex.Sample(smp, input.texRB.xy).x;

	//法線の計算結果、あるいは深度値の計算結果が一定以上ならエッジとみなす。
	if (length(Normal) >= 0.2f || abs(depth) > 0.0001f) {
		output.color.xyz = param_.edgeColor;
		output.color.w = 1.0f;
	}
	else {
		output.color = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
	}
    
    return output;
}