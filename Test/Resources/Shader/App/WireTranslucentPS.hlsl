Texture2D<float32_t4> g_BindlessTextures[] : register(t0, space1);
SamplerState g_Sampler : register(s0);

struct Scene {
    float32_t4x4 viewMatrix;
    float32_t4x4 projectionMatrix;
    uint32_t maskTextureIndex;
    float32_t textureWidth;
    float32_t textureHeight;
};
ConstantBuffer<Scene> g_Scene : register(b0);

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
    float32_t2 maskTexcoord = float32_t2(input.svPosition.x / g_Scene.textureWidth, input.svPosition.y / g_Scene.textureHeight);
    float mask = g_BindlessTextures[g_Scene.maskTextureIndex].Sample(g_Sampler, maskTexcoord).w;
    if (mask > 0.9f) {
        discard;
    }
    
    float3 albedo = g_BindlessTextures[g_Material.albedoMapIndex].Sample(g_Sampler, input.texcoord).xyz;
    albedo *= g_Material.albedo;
    output.color.xyz = albedo * instance.color.xyz;
    output.color.w = instance.color.w;

    return output;
}