struct Instance {
    float4x4 worldMatrix;
    float3 color;
};
ConstantBuffer<Instance> g_Instance : register(b1);

struct PSInput {
    float4 svPosition : SV_POSITION;
};

struct PSOutput {
    float4 color : SV_TARGET0;
};

PSOutput main(PSInput input) {
    PSOutput output;
    output.color.rgb = g_Instance.color;
    output.color.a = 1.0f;
    return output;
}