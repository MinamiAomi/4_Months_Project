#define FLT_MAX 3.402823466e+38F;

struct Constant {
    uint numPoints;
};

struct Point {
    float2 position;
    float3 color;
};

RWTexture2D<float4> g_Texture : register(u0);
ConstantBuffer<Constant> g_Constant : register(b0);
StructuredBuffer<Point> g_Points : register(t0);

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    
    float2 textureSize;
    g_Texture.GetDimensions(textureSize.x, textureSize.y);
    float2 uv = DTid.xy / textureSize;
    
    uint index = 0;
    float minDistance = FLT_MAX;
    
    for (uint i = 0; i < g_Constant.numPoints; ++i) {
        float distance = length(uv - g_Points[i].position);
        if (distance < minDistance) {
            index = i;
            minDistance = distance;
        }
    }
        
    g_Texture[DTid.xy] = float4(g_Points[index].color, 1.0f);
}