#pragma once

#include "../Core/ColorBuffer.h"
#include "../Core/RootSignature.h"
#include "../Core/PipelineState.h"

class CommandContext;

class Voronoi {
public:
    void Initialize(uint32_t width, uint32_t height, uint32_t numPoints);
    void Render(CommandContext& commandContext);

    ColorBuffer& Get() { return colorBuffer_; }

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
    ColorBuffer colorBuffer_;
    uint32_t numPoints_;
};