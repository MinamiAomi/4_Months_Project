Texture2D<float4> g_BindlessTextures[] : register(t0, space1);
SamplerState g_Sampler : register(s0);

struct Scene {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float3 cameraPosition;
};
ConstantBuffer<Scene> g_Scene : register(b0);

struct Instance {
    float4x4 worldMatrix;
    float4x4 worldInverseTransposeMatrix;
    float3 color;
    uint useLighting;
};
struct InstanceOffset {
    uint offset;
};
#ifndef USE_INSTANCING
// GeometryPassVSで使用
ConstantBuffer<Instance> g_Instance : register(b1);
#else
// GeometryPassInstancingVSで使用
StructuredBuffer<Instance> g_Instances : register(t1);
ConstantBuffer<InstanceOffset> g_InstanceOffset : register(b2);
#endif

struct Material {
    float3 albedo;
    float metallic;
    float roughness;
    uint albedoMapIndex;
    uint metallicRoughnessMapIndex;
    uint normalMapIndex;
};
ConstantBuffer<Material> g_Material : register(b3);

#ifdef ENABLE_SKINNING
struct Bone {
    float4x4 boneMatrix;
    float4x4 boneInverseTransposeMatrix;
};
StructuredBuffer<Bone> g_Bones : register(t0);
#endif // ENABLE_SKINNING