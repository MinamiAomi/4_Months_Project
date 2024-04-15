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

    void SetTopColor(const Vector3& topColor) { topColor_ = topColor; }
    void SetBottomColor(const Vector3& bottomColor) { bottomColor_ = bottomColor; }

    const Vector3& GetTopColor() const { return topColor_; }
    const Vector3& GetBottomColor() const { return bottomColor_; }

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;

    Voronoi voronoi_;
    Vector3 topColor_ = { 0.790f,1.0f,0.06f };
    Vector3 bottomColor_ = { 0.220f,1.0f,0.06f };

};