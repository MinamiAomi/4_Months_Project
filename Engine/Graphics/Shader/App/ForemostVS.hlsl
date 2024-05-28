struct Scene {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};
ConstantBuffer<Scene> g_Scene : register(b0);

struct Instance {
    float4x4 worldMatrix;
    float3 color;
};
ConstantBuffer<Instance> g_Instance : register(b1);

struct VSInput {
    float3 position : POSITION0;
};

struct VSOutput {
    float4 svPosition : SV_POSITION;
};

VSOutput main(VSInput input) {
    VSOutput output;

    float4 localPosition = float4(input.position, 1.0f);
    float4 worldPosition = mul(localPosition, g_Instance.worldMatrix);
    output.svPosition = mul(worldPosition, mul(g_Scene.viewMatrix, g_Scene.projectionMatrix));

    return output;
}