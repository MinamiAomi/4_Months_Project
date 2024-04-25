#include "../Lighting.hlsli"

#define TRUE_UINT 1
#define FALSE_UINT 0

#define INVALID_COLOR float3(-1.0f, -1.0f, -1.0f)

//////////////////////////////////////////////////
#define PRIMARY_RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX 0
#define PRIMARY_MISS_SHADER_INDEX 0
//////////////////////////////////////////////////

//////////////////////////////////////////////////
#define SHADOW_RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX 1
#define SHADOW_MISS_SHADER_INDEX 1
//////////////////////////////////////////////////

#define PRIMARY_RAY_ATTRIBUTE (1 << 0)
#define SHADOW_RAY_ATTRIBUTE  (1 << 1)

// シーン
struct Scene {
    float4x4 viewProjectionInverseMatrix;
    float3 cameraPosition;
    float3 sunLightDirection;
};

// 一次レイ用ペイロード
struct PrimaryPayload {
    float shadow;
};
// シャドウレイ用ペイロード
struct ShadowPayload {
    uint isHit;
};

struct Attributes {
    float2 barycentrics;
};

ConstantBuffer<Scene> g_Scene : register(b0);

RaytracingAccelerationStructure g_TLAS : register(t0);
RWTexture2D<float4> g_Shadow : register(u0);


//////////////////////////////////////////////////


// texcoodとdepthからワールド座標を計算
float3 GetWorldPosition(in float2 texcoord, in float depth, in float4x4 viewProjectionInverseMatrix) {
    // xは0~1から-1~1, yは0~1から1~-1に上下反転
    float2 xy = texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 position = float4(xy, depth, 1.0f);
    position = mul(position, viewProjectionInverseMatrix);
    position.xyz /= position.w;
    return position.xyz;
}

[shader("raygeneration")]
void RayGeneration() {
    // レイのインデックス
    uint2 dispatchRaysIndex = DispatchRaysIndex().xy;
    // レイの縦横数
    uint2 dispatchRaysDimensions = DispatchRaysDimensions().xy;
    // テクスチャ座標系を求める    
    float2 texcoord = ((float2) dispatchRaysIndex + 0.5f) / (float2) dispatchRaysDimensions;
    
    //ConstantBuffer<Scene> scene = ResourceDescriptorHeap[descriptorIndex.scene];
    // 近面から遠面へのレイ
    RayDesc primaryRay;
    float3 rayOrigin = GetWorldPosition(texcoord, 0.0f, g_Scene.viewProjectionInverseMatrix);
    primaryRay.Origin = rayOrigin;
    float3 rayDiff = GetWorldPosition(texcoord, 1.0f, g_Scene.viewProjectionInverseMatrix) - rayOrigin;
    primaryRay.Direction = normalize(rayDiff);
    primaryRay.TMin = 0.0f;
    primaryRay.TMax = length(rayDiff);
    // レイを飛ばす
    PrimaryPayload primaryPayload;
    primaryPayload.shadow = 1.0f;
    TraceRay(
        g_TLAS, // RaytracingAccelerationStructure
        RAY_FLAG_CULL_BACK_FACING_TRIANGLES, // RayFlags
        PRIMARY_RAY_ATTRIBUTE, // InstanceInclusionMask
        PRIMARY_RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX, // RayContributionToHitGroupIndex
        0, // MultiplierForGeometryContributionToHitGroupIndex
        PRIMARY_MISS_SHADER_INDEX, // MissShaderIndex
        primaryRay, // Ray
        primaryPayload); // Payload

    float shadow = primaryPayload.shadow;
    g_Shadow[dispatchRaysIndex].rgb = shadow;
    g_Shadow[dispatchRaysIndex].a = 1.0f;
}


//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
   
[shader("closesthit")]
void PrimaryClosestHit(inout PrimaryPayload payload, in Attributes attributes) {

    // レイの情報    
    float hitT = RayTCurrent();
    float3 rayOrigin = WorldRayOrigin();
    float3 rayDirection = WorldRayDirection();
   
    // 影
    bool reciveShadow = (InstanceID() == 1);
    if (reciveShadow) {
        // 衝突点からライトへのレイ
        RayDesc shadowRay;
        // 発射点を法線方向に少しずらす
        shadowRay.Origin = rayOrigin + hitT * rayDirection;
        shadowRay.Direction = -g_Scene.sunLightDirection;
        shadowRay.TMin = 0.01f; // 少し浮かす
        shadowRay.TMax = 100000.0f; // 
        
        ShadowPayload shadowPayload;
        shadowPayload.isHit = 0;
        
        TraceRay(
            g_TLAS,
            0,
            SHADOW_RAY_ATTRIBUTE,
            SHADOW_RAY_CONTRIBUTION_TO_HIT_GROUP_INDEX, // RayContributionToHitGroupIndex
            0, // MultiplierForGeometryContributionToHitGroupIndex
            SHADOW_MISS_SHADER_INDEX, // MissShaderIndex
            shadowRay,
            shadowPayload);
        
        payload.shadow = lerp(1.0f, 0.5f, shadowPayload.isHit);
    }
}


//////////////////////////////////////////////////


[shader("miss")]
void PrimaryMiss(inout PrimaryPayload payload) {
    payload.shadow = 1.0f;
}


//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////


[shader("closesthit")]
void ShadowClosestHit(inout ShadowPayload payload, in Attributes attributes) {
    payload.isHit = TRUE_UINT;
}


//////////////////////////////////////////////////


[shader("miss")]
void ShadowMiss(inout ShadowPayload payload) {
    payload.isHit = FALSE_UINT;
}
