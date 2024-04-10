struct Scene {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float3 cameraPosition;
};
ConstantBuffer<Scene> g_Scene : register(b0);

struct Instance {
    float4x4 worldMatrix;
    float4x4 worldInverseTransposeMatrix;
    uint useLighting;
};
ConstantBuffer<Instance> g_Instance : register(b1);

struct VSInput {
    float3 position : POSITION0;
    float4 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

struct VSOutput {
    float4 svPosition : SV_POSITION;
    float3 worldPosition : POSITION0;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

VSOutput main(VSInput input) {
    VSOutput output;
        
    float4 localPosition = float4(input.position, 1.0f);
    float3 localNormal = input.normal.xyz * 2.0f - 1.0f;
        
    float4 worldPosition = mul(localPosition, g_Instance.worldMatrix);
    output.svPosition = mul(worldPosition, mul(g_Scene.viewMatrix, g_Scene.projectionMatrix));
    output.worldPosition = worldPosition.xyz;
    output.normal = mul(localNormal, (float3x3) g_Instance.worldInverseTransposeMatrix);
    output.texcoord = input.texcoord;
    
    return output;
}