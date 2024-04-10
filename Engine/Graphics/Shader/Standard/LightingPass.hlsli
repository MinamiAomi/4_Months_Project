struct DirectionalLight {
    float3 color;
    float intensity;
    float3 direction;
};

struct PointLight {
    float3 color;
    float intensity;
    float3 position;
    float range;
    float decay;
};

struct SpotLight {
    float3 color;
    float intensity;
    float3 position;
    float angleScale;
    float3 direction;
    float angleOffset;
};

struct LineLight {
    float3 color;
    float intensity;
    float3 origin;
    float range;
    float3 diff;
    float decay;
};

struct Scene {
    float4x4 viewProjectionInverseMatrix;
    float3 cameraPosition;
    uint numDirectionalLights;
    uint numPointLights;
    uint numLineLights;
};

ConstantBuffer<Scene> g_Scene : register(b0);

// G-Buffers
Texture2D<float4> g_Albedo : register(t0);
Texture2D<float2> g_MetallicRoughness : register(t1);
Texture2D<float3> g_Normal : register(t2);
Texture2D<float> g_Depth : register(t3);

StructuredBuffer<DirectionalLight> g_DirectionalLight : register(t0, space1);
StructuredBuffer<PointLight> g_PointLight : register(t1, space1);
StructuredBuffer<LineLight> g_LineLight : register(t2, space1);

SamplerState g_Sampler : register(s0);
