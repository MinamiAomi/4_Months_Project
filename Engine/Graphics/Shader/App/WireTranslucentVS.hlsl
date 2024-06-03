struct Scene {
    float32_t4x4 viewMatrix;
    float32_t4x4 projectionMatrix;
};
ConstantBuffer<Scene> g_Scene : register(b0);

struct Instance {
    float32_t4x4 worldMatrix;
    float32_t4 color;
};
ConstantBuffer<Instance> g_Instance : register(b1);

struct VSInput {
    float32_t3 position : POSITION0;
    float32_t4 normal : NORMAL0;
    float32_t4 tangent : TANGENT0;
    float32_t2 texcoord : TEXCOORD0;
};

struct VSOutput {
    float32_t4 svPosition : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

VSOutput main(VSInput input) {
    VSOutput output;

    float32_t4 localPosition = float32_t4(input.position, 1.0f);

    Instance instance = g_Instance;

    output.svPosition = mul(localPosition, mul(instance.worldMatrix, mul(g_Scene.viewMatrix, g_Scene.projectionMatrix)));
    output.texcoord = input.texcoord;

    return output;
}