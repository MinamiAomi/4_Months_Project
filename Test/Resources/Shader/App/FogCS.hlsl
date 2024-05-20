struct Constant {
    float4x4 projectionInverseMatrix;
    float3 cameraPosition;
    float fogNear;
    float fogLength;
};

RWTexture2D<float4> g_Texture : register(u0);
Texture2D<float4> g_SkyTexture : register(t0);
Texture2D<float4> g_Depth : register(t1);
ConstantBuffer<Constant> g_Constant : register(b0);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    float2 textureSize;
    g_Texture.GetDimensions(textureSize.x, textureSize.y);

    if (!(DTid.x < textureSize.x || DTid.y < textureSize.y)) {
        return;
    }
    float2 uv = DTid.xy / textureSize;
    
    float3 color = g_Texture[DTid.xy].xyz;
    float3 skyTexture = g_SkyTexture[DTid.xy].xyz;
    float depth = g_Depth[DTid.xy].x;
    
    // xは0~1から-1~1, yは0~1から1~-1に上下反転
    float2 xy = uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 tmpPosition = float4(xy, depth, 1.0f);
    tmpPosition = mul(tmpPosition, g_Constant.projectionInverseMatrix);
    float3 position = tmpPosition.xyz / tmpPosition.w;
    
    float distance = length(position);
    float t = saturate((distance - g_Constant.fogNear) / g_Constant.fogLength);
        
    color = lerp(color, skyTexture, t);
       
    g_Texture[DTid.xy].xyz = color;
}