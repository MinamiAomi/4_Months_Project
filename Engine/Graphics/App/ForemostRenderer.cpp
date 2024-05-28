#include "ForemostRenderer.h"

#include "../Core/Helper.h"
#include "../Core/ShaderManager.h"
#include "../Core/CommandContext.h"
#include "../RenderManager.h"

namespace {
    const wchar_t kVertexShader[] = L"App/ForemostVS.hlsl";
    const wchar_t kPixelShader[] = L"App/ForemostPS.hlsl";
}

void ForemostRenderer::Initialize(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) {
    CD3DX12_ROOT_PARAMETER rootParameters[2]{};
    rootParameters[0].InitAsConstantBufferView(0);
    rootParameters[1].InitAsConstantBufferView(1);

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignature_.Create(L"Foremost RootSignature", rootSignatureDesc);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
    pipelineStateDesc.pRootSignature = rootSignature_;

    D3D12_INPUT_ELEMENT_DESC inputElements[] = {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
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
    pipelineStateDesc.DepthStencilState = Helper::DepthStateReadOnly;
    // 前の場合描画
    pipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
    pipelineStateDesc.RasterizerState = Helper::RasterizerDefault;
    pipelineStateDesc.NumRenderTargets = 1;
    pipelineStateDesc.RTVFormats[0] = rtvFormat;
    pipelineStateDesc.DSVFormat = dsvFormat;
    pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateDesc.SampleDesc.Count = 1;

    pipelineState_.Create(L"Foremost PipelineState", pipelineStateDesc);
}

void ForemostRenderer::Render(CommandContext& commandContext, const Camera& camera, const ModelSorter& modelSorter) {
    struct SceneData {
        Matrix4x4 viewMatrix;
        Matrix4x4 projectionMatrix;
    };

    struct InstanceData {
        Matrix4x4 worldMatrix;
        Vector3 color;
    };

    if (modelSorter.GetForemostDrawModels().empty()) { return; }

    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineState_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    SceneData sceneData;
    sceneData.viewMatrix = camera.GetViewMatrix();
    sceneData.projectionMatrix = camera.GetProjectionMatrix();
    commandContext.SetDynamicConstantBufferView(0, sizeof(sceneData), &sceneData);

    for (auto& instance : modelSorter.GetForemostDrawModels()) {
        InstanceData instanceData;
        instanceData.worldMatrix = Matrix4x4::MakeRotationY(180.0f * Math::ToRadian) * instance->GetWorldMatrix();
        instanceData.color = instance->GetForemostColor();
        commandContext.SetDynamicConstantBufferView(1, sizeof(instanceData), &instanceData);

        auto model = instance->GetModel();
        auto skeleton = instance->GetSkeleton();
        auto vbv = model->GetVertexBuffer().GetVertexBufferView();
        if (skeleton) {
            auto skinCluster = RenderManager::GetInstance()->GetSkinningManager().GetSkinCluster(skeleton.get());
            if (skinCluster) {
                vbv = skinCluster->GetSkinnedVertexBuffer().GetVertexBufferView();
            }
        }
        commandContext.SetVertexBuffer(0, vbv);
        commandContext.SetIndexBuffer(model->GetIndexBuffer().GetIndexBufferView());
        commandContext.DrawIndexed((UINT)model->GetNumIndices());
    }
    
}
