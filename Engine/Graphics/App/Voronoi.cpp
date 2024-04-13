#include "Voronoi.h"

#include <cassert>
#include <cmath>
#include <vector>

#include "../Core/Helper.h"
#include "../Core/ShaderManager.h"
#include "../Core/CommandContext.h"
#include "../../Math/MathUtils.h"
#include "../../Math/Random.h"
#include "../../Math/Color.h"

static const wchar_t kComputeShader[] = L"App/VoronoiCS.hlsl";

struct Point {
    Vector2 position;
    Vector3 color;

};

void Voronoi::Initialize(uint32_t width, uint32_t height, uint32_t numPoints) {

    numPoints_ = numPoints;

    CD3DX12_DESCRIPTOR_RANGE uavRange{};
    uavRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

    CD3DX12_ROOT_PARAMETER rootParameters[3]{};
    rootParameters[0].InitAsConstants(1, 0);
    rootParameters[1].InitAsShaderResourceView(0);
    rootParameters[2].InitAsDescriptorTable(1, &uavRange);

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumParameters = _countof(rootParameters);

    rootSignature_.Create(L"Voronoi RootSignature", rootSignatureDesc);


    auto cs = ShaderManager::GetInstance()->Compile(kComputeShader, ShaderType::Compute, 6, 0);
    D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc{};
    pipelineStateDesc.pRootSignature = rootSignature_;
    pipelineStateDesc.CS = CD3DX12_SHADER_BYTECODE(cs->GetBufferPointer(), cs->GetBufferSize());
    pipelineState_.Create(L"Voronoi PipelineState", pipelineStateDesc);

    colorBuffer_.Create(L"Voronoi Result", width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

    CommandContext commandContext;
    commandContext.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);

    commandContext.TransitionResource(colorBuffer_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    commandContext.SetComputeRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineState_);


    commandContext.SetComputeConstants(0, numPoints_);

    std::vector<Point> points(numPoints);
    Random::RandomNumberGenerator rng;
    for (uint32_t i = 0; i < numPoints; ++i) {
        points[i].position = { rng.NextFloatUnit(), rng.NextFloatUnit() };
        points[i].color = Vector3{};
        if (i % 350 == 0) {
            points[i].color = Color::HSVA(rng.NextFloatUnit(), 0.5f, 1.0f);
        }
    }


    commandContext.SetComputeDynamicShaderResourceView(1, points.size() * sizeof(Point), points.data());
    commandContext.SetComputeDescriptorTable(2, colorBuffer_.GetUAV());

    commandContext.Dispatch((UINT)std::floor(width / 8), (UINT)std::floor(height / 8));
    commandContext.UAVBarrier(colorBuffer_);
    commandContext.FlushResourceBarriers();
    commandContext.Finish(true);

}

