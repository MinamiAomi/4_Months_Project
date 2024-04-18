#include "GeometryRenderingPass.h"

#include <unordered_map>
#include <span>

#include "Core/Helper.h"
#include "Core/Graphics.h"
#include "Core/ShaderManager.h"
#include "Core/CommandContext.h"
#include "Math/Camera.h"
#include "Core/SamplerManager.h"
#include "Model.h"
#include "DefaultTextures.h"

namespace {
    const wchar_t kVertexShader[] = L"Standard/GeometryPassVS.hlsl";
    const wchar_t kInstancingVertexShader[] = L"Standard/GeometryPassInstancingVS.hlsl";
    const wchar_t kPixelShader[] = L"Standard/GeometryPassPS.hlsl";
    const wchar_t kInstancingPixelShader[] = L"Standard/GeometryPassInstancingPS.hlsl";
}

void GeometryRenderingPass::Initialize(uint32_t width, uint32_t height) {

    float albedoClearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    albedo_.SetClearColor(albedoClearColor);
    albedo_.Create(L"GeometryRenderingPass Albedo", width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
    float metallicRoughnessClearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    metallicRoughness_.SetClearColor(metallicRoughnessClearColor);
    metallicRoughness_.Create(L"GeometryRenderingPass MetallicRoughness", width, height, DXGI_FORMAT_R16G16_UNORM);
    float normalClearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    normal_.SetClearColor(normalClearColor);
    normal_.Create(L"GeometryRenderingPass Normal", width, height, DXGI_FORMAT_R10G10B10A2_UNORM);
    depth_.Create(L"GeometryRenderingPass Depth", width, height, DXGI_FORMAT_D32_FLOAT);

    // ルートシグネチャ
    {
        CD3DX12_DESCRIPTOR_RANGE srvRange{};
        srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, BINDLESS_RESOURCE_MAX, 0, 1);
        CD3DX12_DESCRIPTOR_RANGE instancingSRVRange{};
        instancingSRVRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

        CD3DX12_ROOT_PARAMETER rootParameters[RootIndex::NumRootParameters]{};
        rootParameters[RootIndex::Scene].InitAsConstantBufferView(0);
        rootParameters[RootIndex::Instance].InitAsConstantBufferView(1);
        rootParameters[RootIndex::Instances].InitAsDescriptorTable(1, &instancingSRVRange);
        rootParameters[RootIndex::InstanceOffset].InitAsConstants(1, 2);
        rootParameters[RootIndex::Material].InitAsConstantBufferView(3);
        rootParameters[RootIndex::BindlessTexture].InitAsDescriptorTable(1, &srvRange);

        CD3DX12_STATIC_SAMPLER_DESC staticSamplerDesc[1]{};
        staticSamplerDesc[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumStaticSamplers = _countof(staticSamplerDesc);
        rootSignatureDesc.pStaticSamplers = staticSamplerDesc;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        rootSignature_.Create(L"GeometryRenderingPass RootSignature", rootSignatureDesc);
    }

    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
        pipelineStateDesc.pRootSignature = rootSignature_;

        D3D12_INPUT_ELEMENT_DESC inputElements[] = {
           { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
           { "NORMAL", 0, DXGI_FORMAT_R10G10B10A2_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
           { "TANGENT", 0, DXGI_FORMAT_R10G10B10A2_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
           { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };
        D3D12_INPUT_LAYOUT_DESC inputLayout{};
        inputLayout.NumElements = _countof(inputElements);
        inputLayout.pInputElementDescs = inputElements;
        pipelineStateDesc.InputLayout = inputLayout;

        auto shaderManager = ShaderManager::GetInstance();
        auto vs = shaderManager->Compile(kVertexShader, ShaderManager::kVertex);
        auto ps = shaderManager->Compile(kPixelShader, ShaderManager::kPixel);
        pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
        pipelineStateDesc.BlendState = Helper::BlendDisable;
        pipelineStateDesc.DepthStencilState = Helper::DepthStateReadWrite;
        pipelineStateDesc.RasterizerState = Helper::RasterizerDefault;
        pipelineStateDesc.NumRenderTargets = 3;
        pipelineStateDesc.RTVFormats[0] = albedo_.GetRTVFormat();
        pipelineStateDesc.RTVFormats[1] = metallicRoughness_.GetRTVFormat();
        pipelineStateDesc.RTVFormats[2] = normal_.GetRTVFormat();
        pipelineStateDesc.DSVFormat = depth_.GetFormat();
        pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipelineStateDesc.SampleDesc.Count = 1;

        pipelineState_.Create(L"GeometryRenderingPass PipelineState", pipelineStateDesc);

        vs = shaderManager->Compile(kInstancingVertexShader, ShaderManager::kVertex);
        ps = shaderManager->Compile(kInstancingPixelShader, ShaderManager::kPixel);
        pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
        instancingPipelineState_.Create(L"GeometryRenderingPass InstancingPipelineState", pipelineStateDesc);
    }

}

void GeometryRenderingPass::Render(CommandContext& commandContext, const Camera& camera) {

    struct SceneData {
        Matrix4x4 viewMatrix;
        Matrix4x4 projectionMatrix;
        Vector3 cameraPosition;
    };

    struct InstanceData {
        Matrix4x4 worldMatrix;
        Matrix4x4 worldInverseTransposeMatrix;
        uint32_t useLighting;
    };


    struct MaterialData {
        Vector3 albedo;
        float metallic;
        float roughness;
        uint32_t albedoMapIndex;
        uint32_t metallicRoughnessMapIndex;
        uint32_t normalMapIndex;
    };

    uint32_t defaultWhiteTextureIndex = DefaultTexture::White.GetSRV().GetIndex();
    uint32_t defaultNormalTextureIndex = DefaultTexture::Normal.GetSRV().GetIndex();

    auto ErrorMaterial = [defaultWhiteTextureIndex, defaultNormalTextureIndex]() {
        MaterialData materialData;
        materialData.albedo = { 0.988f, 0.059f, 0.753f };
        materialData.metallic = 0.0f;
        materialData.roughness = 0.0f;
        materialData.albedoMapIndex = defaultWhiteTextureIndex;
        materialData.metallicRoughnessMapIndex = defaultWhiteTextureIndex;
        materialData.normalMapIndex = defaultNormalTextureIndex;
        return materialData;
    };


    // インスタンスをモデルごとに分けてインスタンシング用のバッファに送る
    auto& instanceList = ModelInstance::GetInstanceList();

    std::unordered_map<Model*, std::vector<ModelInstance*>> drawMap;
    uint32_t drawCount = 0;
    // モデルごとに分ける
    for (const auto& instance : instanceList) {
        // アクティブかつモデルありのみ描画
        if (!instance->IsActive() || !instance->GetModel()) { continue; }
        auto model = instance->GetModel().get();
        drawMap[model].emplace_back(instance);
        ++drawCount;
    }
    // Uploadバッファを割り当てる
    size_t allocateBufferSize = sizeof(InstanceData) * drawCount;
    auto alloc = commandContext.AllocateDynamicBuffer(LinearAllocatorType::Upload, allocateBufferSize);
    std::span<InstanceData> instancesData = { static_cast<InstanceData*>(alloc.cpu), drawCount };
    drawCount = 0;
    // Uploadバッファを埋める
    for (auto& [model, instances] : drawMap) {
        for (auto instance : instances) {
            instancesData[drawCount].worldMatrix = instance->GetWorldMatrix();
            instancesData[drawCount].worldInverseTransposeMatrix = instancesData[drawCount].worldMatrix.Inverse().Transpose();
            instancesData[drawCount].useLighting = instance->UseLighting();
            ++drawCount;
        }
    }

    // 現状のインスタンシングのバッファじゃ足りないので再生成
    if (allocateBufferSize > instancingBuffer_.GetBufferSize()) {
        instancingBuffer_.Create(L"GeoemtryPass InstancingBuffer", drawCount, sizeof(InstanceData));
    }
    // Uploadバッファからコピー
    commandContext.CopyBufferRegion(instancingBuffer_, 0, alloc.resource, alloc.offset, allocateBufferSize);
    commandContext.TransitionResource(instancingBuffer_, D3D12_RESOURCE_STATE_GENERIC_READ);

    commandContext.TransitionResource(albedo_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.TransitionResource(metallicRoughness_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.TransitionResource(normal_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.TransitionResource(depth_, D3D12_RESOURCE_STATE_DEPTH_WRITE);

    commandContext.ClearColor(albedo_);
    commandContext.ClearColor(metallicRoughness_);
    commandContext.ClearColor(normal_);
    commandContext.ClearDepth(depth_);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvs[] = {
        albedo_.GetRTV(),
        metallicRoughness_.GetRTV(),
        normal_.GetRTV()
    };
    commandContext.SetRenderTargets(_countof(rtvs), rtvs, depth_.GetDSV());
    commandContext.SetViewportAndScissorRect(0, 0, albedo_.GetWidth(), albedo_.GetHeight());
    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(instancingPipelineState_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    SceneData sceneData;
    sceneData.viewMatrix = camera.GetViewMatrix();
    sceneData.projectionMatrix = camera.GetProjectionMatrix();
    sceneData.cameraPosition = camera.GetPosition();
    commandContext.SetDynamicConstantBufferView(RootIndex::Scene, sizeof(sceneData), &sceneData);

    commandContext.SetBindlessResource(RootIndex::BindlessTexture);
    commandContext.SetDescriptorTable(RootIndex::Instances, instancingBuffer_.GetSRV());

    uint32_t instanceOffset = 0;
    for (auto& [model, instances] : drawMap) {

        commandContext.SetConstants(RootIndex::InstanceOffset, instanceOffset);

        for (auto& mesh : model->GetMeshes()) {
            MaterialData materialData = ErrorMaterial();
            if (mesh.material) {
                materialData.albedo = mesh.material->albedo;
                materialData.metallic = mesh.material->metallic;
                materialData.roughness = mesh.material->roughness;
                if (mesh.material->albedoMap) { materialData.albedoMapIndex = mesh.material->albedoMap->GetSRV().GetIndex(); }
                if (mesh.material->metallicRoughnessMap) { materialData.metallicRoughnessMapIndex = mesh.material->metallicRoughnessMap->GetSRV().GetIndex(); }
                if (mesh.material->normalMap) { materialData.normalMapIndex = mesh.material->normalMap->GetSRV().GetIndex(); }
            }
            commandContext.SetDynamicConstantBufferView(RootIndex::Material, sizeof(materialData), &materialData);

            commandContext.SetVertexBuffer(0, mesh.vertexBuffer.GetVertexBufferView());
            commandContext.SetIndexBuffer(mesh.indexBuffer.GetIndexBufferView());
            commandContext.DrawIndexedInstanced((UINT)mesh.indices.size(), (UINT)instances.size());
        }

        instanceOffset += static_cast<uint32_t>(instances.size());
    }


    //for (auto const instance : instances) {
    //    // アクティブかつモデルありのみ描画
    //    if (!instance->IsActive() || !instance->GetModel()) { continue; }

    //    InstanceData instanceData;
    //    instanceData.worldMatrix = instance->GetWorldMatrix();
    //    instanceData.worldInverseTransposeMatrix = instance->GetWorldMatrix().Inverse().Transpose();
    //    instanceData.useLighting = instance->UseLighting() ? 1 : 0;
    //    commandContext.SetDynamicConstantBufferView(RootIndex::Instance, sizeof(instanceData), &instanceData);

    //    for (auto& mesh : instance->GetModel()->GetMeshes()) {
    //        MaterialData materialData = ErrorMaterial();
    //        if (mesh.material) {
    //            materialData.albedo = mesh.material->albedo;
    //            materialData.metallic = mesh.material->metallic;
    //            materialData.roughness = mesh.material->roughness;
    //            if (mesh.material->albedoMap) { materialData.albedoMapIndex = mesh.material->albedoMap->GetSRV().GetIndex(); }
    //            if (mesh.material->metallicRoughnessMap) { materialData.metallicRoughnessMapIndex = mesh.material->metallicRoughnessMap->GetSRV().GetIndex(); }
    //            if (mesh.material->normalMap) { materialData.normalMapIndex = mesh.material->normalMap->GetSRV().GetIndex(); }
    //        }
    //        commandContext.SetDynamicConstantBufferView(RootIndex::Material, sizeof(materialData), &materialData);

    //        commandContext.SetVertexBuffer(0, mesh.vertexBuffer.GetVertexBufferView());
    //        commandContext.SetIndexBuffer(mesh.indexBuffer.GetIndexBufferView());
    //        commandContext.DrawIndexed((UINT)mesh.indices.size());
    //    }
    //}
}
