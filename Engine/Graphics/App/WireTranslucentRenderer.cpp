#include "WireTranslucentRenderer.h"

#include <unordered_map>
#include <span>

#include "../Core/Helper.h"
#include "../Core/Graphics.h"
#include "../Core/ShaderManager.h"
#include "../Core/SamplerManager.h"
#include "../DefaultTextures.h"
#include "../RenderManager.h"

namespace {
    const wchar_t kVertexShader[] = L"App/WireTranslucentVS.hlsl";
    const wchar_t kPixelShader[] = L"App/WireTranslucentPS.hlsl";
}

void WireTranslucentRenderer::Initialize(DXGI_FORMAT rtvFormat) {
    // ルートシグネチャ
    {
        CD3DX12_DESCRIPTOR_RANGE srvRange{};
        srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, BINDLESS_RESOURCE_MAX, 0, 1);

        CD3DX12_ROOT_PARAMETER rootParameters[RootIndex::NumRootParameters]{};
        rootParameters[RootIndex::Scene].InitAsConstantBufferView(0);
        rootParameters[RootIndex::Instance].InitAsConstantBufferView(1);
        rootParameters[RootIndex::Material].InitAsConstantBufferView(2);
        rootParameters[RootIndex::BindlessTexture].InitAsDescriptorTable(1, &srvRange);

        CD3DX12_STATIC_SAMPLER_DESC staticSamplerDesc[1]{};
        staticSamplerDesc[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumStaticSamplers = _countof(staticSamplerDesc);
        rootSignatureDesc.pStaticSamplers = staticSamplerDesc;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        rootSignature_.Create(L"WireTranslucent RootSignature", rootSignatureDesc);
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
        pipelineStateDesc.DepthStencilState = Helper::DepthStateDisabled;
        //pipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
        pipelineStateDesc.RasterizerState = Helper::RasterizerDefault;
        pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
        pipelineStateDesc.NumRenderTargets = 1;
        pipelineStateDesc.RTVFormats[0] = rtvFormat;
        pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipelineStateDesc.SampleDesc.Count = 1;

        pipelineState_.Create(L"WireTranslucent PipelineState", pipelineStateDesc);
    }
}

void WireTranslucentRenderer::Render(CommandContext& commandContext, const Camera& camera, ColorBuffer& colorBuffer) {
    struct SceneData {
        Matrix4x4 viewMatrix;
        Matrix4x4 projectionMatrix;
    };
    struct InstanceData {
        Matrix4x4 worldMatrix;
        Vector4 color;
    };
    struct MaterialData {
        Vector3 albedo;
        uint32_t albedoMapIndex;
    };
    
    if (!model_ || !model_->GetModel() || !model_->IsActive()) { return; }

    commandContext.TransitionResource(colorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(colorBuffer.GetRTV());
    commandContext.SetViewportAndScissorRect(0, 0, colorBuffer.GetWidth(), colorBuffer.GetHeight());
    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineState_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    SceneData sceneData;
    sceneData.viewMatrix = camera.GetViewMatrix();
    sceneData.projectionMatrix = camera.GetProjectionMatrix();
    commandContext.SetDynamicConstantBufferView(RootIndex::Scene, sizeof(sceneData), &sceneData);
    
    InstanceData instanceData;
    instanceData.worldMatrix = Matrix4x4::MakeRotationY(Math::Pi) * model_->GetWorldMatrix();
    instanceData.color = Vector4(model_->GetColor(), model_->GetAlpha());
    commandContext.SetDynamicConstantBufferView(RootIndex::Instance, sizeof(instanceData), &instanceData);

    commandContext.SetBindlessResource(RootIndex::BindlessTexture);
    auto model = model_->GetModel();
    for (auto& mesh : model->GetMeshes()) {
        MaterialData materialData;
        if (mesh.material < model->GetMaterials().size()) {
            auto& material = model->GetMaterials()[mesh.material];
            materialData.albedo = material.albedo;
            if (material.albedoMap) { materialData.albedoMapIndex = material.albedoMap->GetSRV().GetIndex(); }
        }
        commandContext.SetDynamicConstantBufferView(RootIndex::Material, sizeof(materialData), &materialData);

        auto skeleton = model_->GetSkeleton().get();
        auto vbv = model->GetVertexBuffer().GetVertexBufferView();
        if (skeleton) {
            auto skinCluster = RenderManager::GetInstance()->GetSkinningManager().GetSkinCluster(skeleton);
            if (skinCluster) {
                vbv = skinCluster->GetSkinnedVertexBuffer().GetVertexBufferView();
            }
        }
        commandContext.SetVertexBuffer(0, vbv);
        commandContext.SetIndexBuffer(model->GetIndexBuffer().GetIndexBufferView());
        commandContext.DrawIndexed((UINT)mesh.indexCount,mesh.indexOffset, mesh.vertexOffset);
    }
}
