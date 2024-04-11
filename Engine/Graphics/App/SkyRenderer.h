#pragma once

#include "../Core/RootSignature.h"
#include "../Core/PipelineState.h"
#include "../Core/CommandContext.h"

#include "Math/MathUtils.h"
#include "Math/Camera.h"

#include "Voronoi.h"

class SkyRenderer {
public:
    void Initialize(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
    void Render(CommandContext& commandContext, const Camera& camera, Matrix4x4 worldMatrix);

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;

    Voronoi voronoi_;
    Vector3 topColor_;
    Vector3 bottomColor_;

};