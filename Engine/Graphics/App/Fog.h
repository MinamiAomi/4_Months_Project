#pragma once

#include "../Core/ColorBuffer.h"
#include "../Core/DepthBuffer.h"
#include "../Core/RootSignature.h"
#include "../Core/PipelineState.h"
#include "../../Math/Camera.h"

class CommandContext;

class Fog {
public:
    struct RootParameter {
        enum Enum {
            Texture,
            SkyTexture,
            Depth,
            Constant,
            NumRootParameter
        };
    };


    void Initialize();
    void Render(CommandContext& commandContext, const Camera& camera, ColorBuffer& texture, ColorBuffer& skyTexture, DepthBuffer& depth);

    void SetNear(float fogNear) { near_ = fogNear; }
    void SetFar(float fogFar) { far_ = fogFar; }
    float GetNear() const { return near_; }
    float GetFar() const { return far_; }

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
    float near_ = 100.0f;
    float far_ = 700.0f;
};