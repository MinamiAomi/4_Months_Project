#pragma once

#include "../Core/ColorBuffer.h"
#include "../Core/DepthBuffer.h"
#include "../Core/RootSignature.h"
#include "../Core/PipelineState.h"
#include "../../Math/Camera.h"
#include "../ModelSorter.h"

class CommandContext;

class ForemostRenderer {
public:
    void Initialize(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
    void Render(CommandContext& commandContext, const Camera& camera, const ModelSorter& modelSorter);

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
};