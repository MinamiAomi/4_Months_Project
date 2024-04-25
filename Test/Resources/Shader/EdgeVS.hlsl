struct VSInput {
    uint vertexID : SV_VertexID;
};

Texture2D<float4> colorTex : register(t0);

struct VSOutput {
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

// MiniEngine　ScreenQuadCommonVS.hlsl
VSOutput main(VSInput input) {
    VSOutput output;

	float2 texSize;
	colorTex.GetDimensions(texSize.x, texSize.y);

	float32_t2 pixelSize = 1.0f / texSize;
	
	//真ん中のピクセル
	output.texCenter = float2(uint2(input.vertexID, input.vertexID << 1) & 2);
	//右上のピクセル
	output.texLT.xy = output.texCenter + float2(pixelSize.x, -pixelSize.y);
	//上のピクセル
	output.texMT.xy = output.texCenter + float2(0.0f, -pixelSize.y);
	//左上のピクセル
	output.texRT.xy = output.texCenter + float2(-pixelSize.x, -pixelSize.y);
	//右のピクセル
	output.texLM.xy = output.texCenter + float2(pixelSize.x, 0.0f);
	//左のピクセル
	output.texRM.xy = output.texCenter + float2(-pixelSize.x, 0.0f);
	//上のピクセル
	output.texLB.xy = output.texCenter + float2(pixelSize.x, pixelSize.y);
	//上のピクセル
	output.texMB.xy = output.texCenter + float2(0.0f, pixelSize.y);
	//上のピクセル
	output.texRB.xy = output.texCenter + float2(-pixelSize.x, pixelSize.y);

    output.position = float4(lerp(float2(-1.0f, 1.0f), float2(1.0f, -1.0f), output.texCenter), 0.0f, 1.0f);

    return output;
}