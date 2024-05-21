struct PSInput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

SamplerState smp : register(s0);

struct PSOutput {
    float4 color : SV_TARGET0;
};

struct T {
	float32_t value;
};
ConstantBuffer<T> t  : register(b0);

PSOutput main(PSInput input) {
    PSOutput output;
	float alpha = t.value;
	output.color = float32_t4( 1.0f,1.0f,1.0f,alpha );
    return output;
}