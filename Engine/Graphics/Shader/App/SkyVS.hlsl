struct Scene {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 worldMatrix;
    float4x4 worldInverseTransposeMatrix;
    float3 topColor;
    float3 bottomColor;
};
ConstantBuffer<Scene> g_Scene : register(b0);

struct VSInput {
    float3 position : POSITION0;
    float3 normal : NORMAL0;
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
    float3 localNormal = input.normal.xyz;
        
    float4 worldPosition = mul(localPosition, g_Scene.worldMatrix);
    output.svPosition = mul(worldPosition, mul(g_Scene.viewMatrix, g_Scene.projectionMatrix));
    output.worldPosition = worldPosition.xyz;
    output.normal = mul(localNormal, (float3x3) g_Scene.worldInverseTransposeMatrix);
    output.texcoord = input.texcoord;
    
    return output;
}