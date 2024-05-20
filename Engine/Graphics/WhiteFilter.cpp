#include "WhiteFilter.h"

#include <cassert>

#include "Core/CommandContext.h"
#include "Core/Helper.h"
#include "Core/ShaderManager.h"

#include "GeometryRenderingPass.h"
#include "ImGuiManager.h"

#include "../Test/GameClearMovie.h"

static const char kWhiteFilterVS[] = "ScreenQuadVS.hlsl";
static const char kWhiteFilterPS[] = "WhiteFilterPS.hlsl";

void WhiteFilter::Initialize(ColorBuffer& originalTexture) {
    originalTexture_ = &originalTexture;
    whiteFilterTexture_.Create(L"whiteFilterTexture", originalTexture_->GetWidth(), originalTexture_->GetHeight(), originalTexture_->GetFormat());

   
    //Effect
    {
        CD3DX12_DESCRIPTOR_RANGE ranges[static_cast<uint32_t>(RootParameter::ParameterNum)]{};
        for (uint32_t i = 0; i < static_cast<uint32_t>(RootParameter::ParameterNum); ++i) {
            ranges[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i);
        }

        CD3DX12_ROOT_PARAMETER rootParameters[static_cast<uint32_t>(RootParameter::ParameterNum)]{};
        rootParameters[static_cast<uint32_t>(RootParameter::kT)].InitAsConstants(1, 0);

        CD3DX12_STATIC_SAMPLER_DESC staticSamplerDesc(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        rootSignatureDesc.pStaticSamplers = &staticSamplerDesc;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        rootSignature_.Create(L"WhiteFilter RootSignature", rootSignatureDesc);
    }

    {
        DXGI_FORMAT format = originalTexture_->GetFormat();

        auto shaderManager = ShaderManager::GetInstance();

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
        psoDesc.pRootSignature = rootSignature_;

        auto vs = shaderManager->Compile(kWhiteFilterVS, ShaderManager::kVertex);
        auto ps = shaderManager->Compile(kWhiteFilterPS, ShaderManager::kPixel);
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());


        psoDesc.BlendState = Helper::BlendAlpha;
        psoDesc.RasterizerState = Helper::RasterizerNoCull;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = format;
        psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.SampleDesc.Count = 1;
        pipelineState_.Create(L"WhiteFilter PSO", psoDesc);
    }
}

void WhiteFilter::Render(CommandContext& commandContext)
{
    if (GameClearMovie::t_ != 0.0f) {
        //effect
        commandContext.TransitionResource(*originalTexture_, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandContext.SetRenderTarget(originalTexture_->GetRTV());
        commandContext.SetViewportAndScissorRect(0, 0, originalTexture_->GetWidth(), originalTexture_->GetHeight());

        commandContext.SetRootSignature(rootSignature_);
        commandContext.SetPipelineState(pipelineState_);
        commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        commandContext.SetConstants(static_cast<UINT>(RootParameter::kT), GameClearMovie::t_);

        commandContext.Draw(3);
    }
}
