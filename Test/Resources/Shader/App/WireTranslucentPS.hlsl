Texture2D<float32_t4> g_BindlessTextures[] : register(t0, space1);
SamplerState g_Sampler : register(s0);

struct Instance {
    float32_t4x4 worldMatrix;
    float32_t4 color;
};
ConstantBuffer<Instance> g_Instance : register(b1);

struct Material {
    float32_t3 albedo;
    uint32_t albedoMapIndex;
};
ConstantBuffer<Material> g_Material : register(b2);

struct PSInput {
    float4 svPosition : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float4 color : SV_TARGET0;
};

PSOutput main(PSInput input) {

    PSOutput output;

    Instance instance = g_Instance;

    float3 albedo = g_BindlessTextures[g_Material.albedoMapIndex].Sample(g_Sampler, input.texcoord).xyz;
    albedo *= g_Material.albedo;
    output.color.xyz = albedo * instance.color.xyz;
    output.color.w = instance.color.w;

    return output;
}