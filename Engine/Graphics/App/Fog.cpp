#include "Fog.h"

#include <cassert>

#include "../Core/Helper.h"
#include "../Core/ShaderManager.h"
#include "../Core/CommandContext.h"
#include "../../Math/MathUtils.h"
#include "../../Math/Color.h"

static const wchar_t kComputeShader[] = L"App/FogCS.hlsl";

void Fog::Initialize() {

    CD3DX12_DESCRIPTOR_RANGE uavRange{};
    uavRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
    CD3DX12_DESCRIPTOR_RANGE srvRange{};
    srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    CD3DX12_DESCRIPTOR_RANGE srvRange2{};
    srvRange2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

    CD3DX12_ROOT_PARAMETER rootParameters[RootParameter::NumRootParameter]{};
    rootParameters[RootParameter::Texture].InitAsDescriptorTable(1, &uavRange);
    rootParameters[RootParameter::SkyTexture].InitAsDescriptorTable(1, &srvRange);
    rootParameters[RootParameter::Depth].InitAsDescriptorTable(1, &srvRange2);
    rootParameters[RootParameter::Constant].InitAsConstantBufferView(0);

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumParameters = _countof(rootParameters);

    rootSignature_.Create(L"Fog RootSignature", rootSignatureDesc);


    auto cs = ShaderManager::GetInstance()->Compile(kComputeShader, ShaderType::Compute, 6, 0);
    D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc{};
    pipelineStateDesc.pRootSignature = rootSignature_;
    pipelineStateDesc.CS = CD3DX12_SHADER_BYTECODE(cs->GetBufferPointer(), cs->GetBufferSize());
    pipelineState_.Create(L"Fog PipelineState", pipelineStateDesc);
}

void Fog::Render(CommandContext& commandContext, const Camera& camera, ColorBuffer& texture, ColorBuffer& skyTexture, DepthBuffer& depth) {

    struct Constant {
        Matrix4x4 projectionInverseMatrix;
        Vector3 cameraPosition;
        float fogNear;
        float fogLength;
    };

    commandContext.TransitionResource(texture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    commandContext.TransitionResource(skyTexture, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(depth, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

    commandContext.SetComputeRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineState_);

    commandContext.SetComputeDescriptorTable(RootParameter::Texture, texture.GetUAV());
    commandContext.SetComputeDescriptorTable(RootParameter::SkyTexture, skyTexture.GetSRV());
    commandContext.SetComputeDescriptorTable(RootParameter::Depth, depth.GetSRV());

    Constant constant;
    constant.projectionInverseMatrix = camera.GetProjectionMatrix().Inverse();
    constant.cameraPosition = camera.GetPosition();
    constant.fogNear = near_;
    constant.fogLength = far_ - near_;
    commandContext.SetComputeDynamicConstantBufferView(RootParameter::Constant, sizeof(constant), &constant);

    commandContext.Dispatch((UINT)std::floor(texture.GetWidth() / 8), (UINT)std::floor(texture.GetHeight() / 8));
    commandContext.UAVBarrier(texture);
    commandContext.FlushResourceBarriers();
}
