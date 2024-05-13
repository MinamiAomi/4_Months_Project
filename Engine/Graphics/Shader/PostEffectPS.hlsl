#define NUM_SAMPLE 10

struct Constant {
    float32_t2 center;
    float32_t blurWidth;
};
ConstantBuffer<Constant> g_Constant : register(b0);
Texture2D<float32_t4> g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct PSInput {
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float32_t4 color : SV_TARGET0;
};

PSOutput main(PSInput input) {
    PSOutput output;
    
    float32_t2 direction = input.texcoord - g_Constant.center;
    float32_t3 outputColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (int32_t sampleIndex = 0; sampleIndex < NUM_SAMPLES; ++sampleIndex) {
        float32_t2 texcoord = input.texcoord * direction * g_Constant.blurWidth * float32_t(sampleIndex);
        outputColor += g_Texture.Sample(g_Sampler, texcoord).rgb;
    }
    outputColor *= rcp(NUM_SAMPLES);
   
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    
    return output;
}