#include "Edge.h"

#include <cassert>

#include "Core/CommandContext.h"
#include "Core/Helper.h"
#include "Core/ShaderManager.h"

#include "GeometryRenderingPass.h"
#include "ImGuiManager.h"

static const char kEdgeVS[] = "EdgeVS.hlsl";
static const char kEdgePS[] = "EdgePS.hlsl";

void Edge::Initialize(ColorBuffer* originalTexture) {
    edgeTexture_.Create(L"edgeTexture", originalTexture->GetWidth(), originalTexture->GetHeight(), originalTexture->GetFormat());
    originalTexture_ = originalTexture;
    {
        CD3DX12_DESCRIPTOR_RANGE ranges[static_cast<uint32_t>(RootParameter::ParameterNum)]{};
        for (uint32_t i = 0; i < static_cast<uint32_t>(RootParameter::ParameterNum); ++i) {
            ranges[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i);
        }

        CD3DX12_ROOT_PARAMETER rootParameters[static_cast<uint32_t>(RootParameter::ParameterNum)]{};
        rootParameters[static_cast<uint32_t>(RootParameter::kColorTexture)].InitAsDescriptorTable(1, &ranges[static_cast<uint32_t>(RootParameter::kColorTexture)]);
        rootParameters[static_cast<uint32_t>(RootParameter::kNormalTexture)].InitAsDescriptorTable(1, &ranges[static_cast<uint32_t>(RootParameter::kNormalTexture)]);
        rootParameters[static_cast<uint32_t>(RootParameter::kDepthTexture)].InitAsDescriptorTable(1, &ranges[static_cast<uint32_t>(RootParameter::kDepthTexture)]);
        rootParameters[static_cast<uint32_t>(RootParameter::kEdgeColor)].InitAsConstants(3, 0);

        CD3DX12_STATIC_SAMPLER_DESC staticSamplerDesc(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        rootSignatureDesc.pStaticSamplers = &staticSamplerDesc;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        rootSignature_.Create(L"Edge RootSignature", rootSignatureDesc);
    }

    {
        DXGI_FORMAT format = originalTexture->GetFormat();

        auto shaderManager = ShaderManager::GetInstance();

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
        psoDesc.pRootSignature = rootSignature_;

        auto vs = shaderManager->Compile(kEdgeVS, ShaderManager::kVertex);
        auto ps = shaderManager->Compile(kEdgePS, ShaderManager::kPixel);
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());


        psoDesc.BlendState = Helper::BlendDisable;
        psoDesc.RasterizerState = Helper::RasterizerNoCull;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = format;
        psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.SampleDesc.Count = 1;
        pipelineState_.Create(L"Edge PSO", psoDesc);
    
        ps = shaderManager->Compile(kEdgePS, ShaderManager::kPixel);
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
    }
}

void Edge::Render(CommandContext& commandContext, GeometryRenderingPass& geometryRenderingPass) {
       
    commandContext.TransitionResource(edgeTexture_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.ClearColor(edgeTexture_);
    commandContext.SetRenderTarget(edgeTexture_.GetRTV());
    commandContext.SetViewportAndScissorRect(0, 0, edgeTexture_.GetWidth(), edgeTexture_.GetHeight());

    commandContext.TransitionResource(geometryRenderingPass.GetAlbedo(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(geometryRenderingPass.GetNormal(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(geometryRenderingPass.GetDepth(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineState_);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kColorTexture), geometryRenderingPass.GetAlbedo().GetSRV());
    commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kNormalTexture), geometryRenderingPass.GetNormal().GetSRV());
    commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kDepthTexture), geometryRenderingPass.GetDepth().GetSRV());

    commandContext.SetConstants(static_cast<UINT>(RootParameter::kEdgeColor), edgeColor_.x, edgeColor_.y, edgeColor_.z);
    commandContext.Draw(3);

}
