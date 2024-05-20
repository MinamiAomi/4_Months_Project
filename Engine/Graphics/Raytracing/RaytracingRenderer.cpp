#include "RaytracingRenderer.h"

#include "../Core/Graphics.h"
#include "../Core/Helper.h"
#include "../Core/ShaderManager.h"
#include "../Core/CommandContext.h"
#include "../LightManager.h"
#include "../Model.h"
#include "../DefaultTextures.h"
#include "../Core/SamplerManager.h"
#include "../RenderManager.h"

#define PRIMARY_RAY_ATTRIBUTE (1 << 0)
#define SHADOW_RAY_ATTRIBUTE  (1 << 1)

static const wchar_t kRaytracingShader[] = L"Raytracing/Raytracing.hlsl";
static const wchar_t kRayGenerationName[] = L"RayGeneration";
static const wchar_t kPrimaryMissName[] = L"PrimaryMiss";
static const wchar_t kShadowMissName[] = L"ShadowMiss";
static const wchar_t kPrimaryRayClosestHitName[] = L"PrimaryClosestHit";
static const wchar_t kShadowRayClosestHitName[] = L"ShadowClosestHit";
static const wchar_t kPrimaryRayHitGroupName[] = L"PrimaryHitGroup";
static const wchar_t kShadowRayHitGroupName[] = L"ShadowHitGroup";

void RaytracingRenderer::Create(uint32_t width, uint32_t height) {
    CreateRootSignature();
    CreateStateObject();
    CreateShaderTables();
    shadowBuffer_.Create(L"RaytracingRenderer ResultBuffer", width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
}

void RaytracingRenderer::Render(CommandContext& commandContext, const Camera& camera, const DirectionalLight& sunLight) {
    auto commandList = commandContext.GetDXRCommandList();

    // シーン定数
    struct Scene {
        Matrix4x4 viewProjectionInverseMatrix;
        Vector3 cameraPosition;
        float pad;
        Vector3 sunLightDirection;
    };

    // TLASを生成
    bool draw = BuildScene(commandContext);
    if (!draw) { return; }

    // シーン定数を送る
    Scene scene;
    scene.viewProjectionInverseMatrix = camera.GetViewProjectionMatrix().Inverse();
    scene.cameraPosition = camera.GetPosition();
    scene.sunLightDirection = sunLight.direction;
    auto sceneCB = commandContext.TransfarUploadBuffer(sizeof(scene), &scene);


    commandContext.TransitionResource(shadowBuffer_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    commandContext.FlushResourceBarriers();

    commandList->SetComputeRootSignature(globalRootSignature_);
    commandList->SetPipelineState1(stateObject_);

    commandList->SetComputeRootConstantBufferView(0, sceneCB);
    commandList->SetComputeRootDescriptorTable(1, tlas_.GetSRV());
    commandList->SetComputeRootDescriptorTable(2, shadowBuffer_.GetUAV());

    D3D12_DISPATCH_RAYS_DESC rayDesc{};
    rayDesc.RayGenerationShaderRecord.StartAddress = rayGenerationShaderTable_.GetGPUVirtualAddress();
    rayDesc.RayGenerationShaderRecord.SizeInBytes = rayGenerationShaderTable_.GetBufferSize();
    rayDesc.MissShaderTable.StartAddress = missShaderTable_.GetGPUVirtualAddress();
    rayDesc.MissShaderTable.SizeInBytes = missShaderTable_.GetBufferSize();
    rayDesc.MissShaderTable.StrideInBytes = missShaderTable_.GetShaderRecordSize();
    rayDesc.HitGroupTable.StartAddress = hitGroupShaderTable_.GetGPUVirtualAddress();
    rayDesc.HitGroupTable.SizeInBytes = hitGroupShaderTable_.GetBufferSize();
    rayDesc.HitGroupTable.StrideInBytes = hitGroupShaderTable_.GetShaderRecordSize();
    rayDesc.Width = shadowBuffer_.GetWidth();
    rayDesc.Height = shadowBuffer_.GetHeight();
    rayDesc.Depth = 1;
    commandList->DispatchRays(&rayDesc);

    commandContext.UAVBarrier(shadowBuffer_);
    commandContext.FlushResourceBarriers();
}

void RaytracingRenderer::CreateRootSignature() {

    {

        CD3DX12_DESCRIPTOR_RANGE descriptorRanges[2]{};
        descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        descriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);


        CD3DX12_ROOT_PARAMETER rootParameters[3]{};
        rootParameters[0].InitAsConstantBufferView(0);
        rootParameters[1].InitAsDescriptorTable(1, descriptorRanges + 0);
        rootParameters[2].InitAsDescriptorTable(1, descriptorRanges + 1);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        globalRootSignature_.Create(L"GlobalRootSignatures", rootSignatureDesc);
    }

}

void RaytracingRenderer::CreateStateObject() {
    CD3DX12_STATE_OBJECT_DESC stateObjectDesc{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

    // 1.DXILLib
    auto shader = ShaderManager::GetInstance()->Compile(kRaytracingShader, ShaderType::Library, 6, 4);
    CD3DX12_SHADER_BYTECODE shaderByteCode(shader->GetBufferPointer(), shader->GetBufferSize());
    auto dxilLibSubobject = stateObjectDesc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
    dxilLibSubobject->SetDXILLibrary(&shaderByteCode);
    dxilLibSubobject->DefineExport(kRayGenerationName);
    dxilLibSubobject->DefineExport(kPrimaryMissName);
    dxilLibSubobject->DefineExport(kShadowMissName);
    dxilLibSubobject->DefineExport(kPrimaryRayClosestHitName);
    dxilLibSubobject->DefineExport(kShadowRayClosestHitName);

    // 2.一次レイヒットグループ
    auto primaryRayHitGroup = stateObjectDesc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    primaryRayHitGroup->SetClosestHitShaderImport(kPrimaryRayClosestHitName);
    primaryRayHitGroup->SetHitGroupExport(kPrimaryRayHitGroupName);
    primaryRayHitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

    // 5.シャドウレイヒットグループ
    auto shadowRayHitGroup = stateObjectDesc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    shadowRayHitGroup->SetClosestHitShaderImport(kShadowRayClosestHitName);
    shadowRayHitGroup->SetHitGroupExport(kShadowRayHitGroupName);
    shadowRayHitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

    // 6.シェーダーコンフィグ
    auto shaderConfig = stateObjectDesc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
    size_t maxPayloadSize = 1 * sizeof(float);      // 最大ペイロードサイズ
    size_t maxAttributeSize = 2 * sizeof(float);   // 最大アトリビュートサイズ
    shaderConfig->Config((UINT)maxPayloadSize, (UINT)maxAttributeSize);

    // 7.パイプラインコンフィグ
    auto pipelineConfig = stateObjectDesc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
    uint32_t maxTraceRecursionDepth = 2; // 一次レイ, シャドウレイ
    pipelineConfig->Config(maxTraceRecursionDepth);

    // 8.グローバルルートシグネチャ
    auto globalRootSignature = stateObjectDesc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    globalRootSignature->SetRootSignature(globalRootSignature_);

    stateObject_.Create(L"RaytracingStateObject", stateObjectDesc);
}

void RaytracingRenderer::CreateShaderTables() {

    {
        Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
        stateObject_.Get().As(&stateObjectProperties);

        auto InsertIdentifier = [&](const wchar_t* name) {
            identifierMap_[name] = stateObjectProperties->GetShaderIdentifier(name);
            };
        InsertIdentifier(kRayGenerationName);
        InsertIdentifier(kPrimaryRayHitGroupName);
        InsertIdentifier(kShadowRayHitGroupName);
        InsertIdentifier(kPrimaryMissName);
        InsertIdentifier(kShadowMissName);
    }

    {
        ShaderRecord rayGenerationShaderRecord(identifierMap_[kRayGenerationName]);
        rayGenerationShaderTable_.Create(L"RaytracingRenderer RayGenerationShaderTable", &rayGenerationShaderRecord, 1);
    }
    {
        std::vector<ShaderRecord> shaderRecords;
        shaderRecords.emplace_back(identifierMap_[kPrimaryRayHitGroupName]);
        shaderRecords.emplace_back(identifierMap_[kShadowRayHitGroupName]);
        hitGroupShaderTable_.Create(L"RaytracingRenderer HitGroupShaderTable", shaderRecords.data(), (UINT)shaderRecords.size());
    }
    // ヒットグループは毎フレーム更新
    {
        std::vector<ShaderRecord> shaderRecords;
        shaderRecords.emplace_back(identifierMap_[kPrimaryMissName]);
        shaderRecords.emplace_back(identifierMap_[kShadowMissName]);
        missShaderTable_.Create(L"RaytracingRenderer MissShaderTable", shaderRecords.data(), (UINT)shaderRecords.size());
    }
}

bool RaytracingRenderer::BuildScene(CommandContext& commandContext) {
    auto& instanceList = ModelInstance::GetInstanceList();

    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDescs;
    instanceDescs.reserve(instanceList.size());

    size_t drawCount = 0;
    // レイトレで使用するオブジェクトをインスタンスデスクに登録
    for (auto& instance : instanceList) {
        if (!(instance->IsActive() && instance->GetModel())) {
            continue;
        }

        auto model = instance->GetModel();
        auto& desc = instanceDescs.emplace_back();
        Matrix4x4 mat = Matrix4x4::MakeRotationY(180.0f * Math::ToRadian) * instance->GetWorldMatrix();

        for (uint32_t y = 0; y < 3; ++y) {
            for (uint32_t x = 0; x < 4; ++x) {
                desc.Transform[y][x] = mat.m[x][y];
            }
        }
        desc.InstanceID = instance->ReciveShadow() ? 1 : 0;
        desc.InstanceMask = PRIMARY_RAY_ATTRIBUTE;
        if (instance->CastShadow()) {
            desc.InstanceMask |= SHADOW_RAY_ATTRIBUTE;
        }
        desc.InstanceContributionToHitGroupIndex = 0;
        desc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
        desc.AccelerationStructure = model->GetBLAS().GetGPUVirtualAddress();
      /*  auto skeleton = instance->GetSkeleton().get();
        if (skeleton) {
            auto skinCluster = RenderManager::GetInstance()->GetSkinningManager().GetSkinCluster(skeleton);
            if (skinCluster) {
                desc.AccelerationStructure = skinCluster->.GetVertexBufferView();
            }
        }*/
        drawCount++;
    }

    if (drawCount == 0) {
        return false;
    }

    tlas_.Create(L"RaytracingRenderer TLAS", commandContext, instanceDescs.data(), instanceDescs.size());
    return true;
}
