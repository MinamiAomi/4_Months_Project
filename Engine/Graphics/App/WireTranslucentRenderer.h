#pragma once

#include <vector>

#include "../Core/RootSignature.h"
#include "../Core/PipelineState.h"
#include "../Core/CommandContext.h"

#include "../Model.h"
#include "Math/MathUtils.h"
#include "Math/Camera.h"

class WireTranslucentRenderer {
public:
    struct RootIndex {
        enum Parameters {
            Scene,
            Instance,
            Material,
            BindlessTexture,

            NumRootParameters
        };
    };

    void Initialize(DXGI_FORMAT rtvFormat);
    void Render(CommandContext& commandContext, const Camera& camera, ColorBuffer& colorBuffer);

    void SetModel(ModelInstance* model) { model_ = model; }

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
    ModelInstance* model_;
};