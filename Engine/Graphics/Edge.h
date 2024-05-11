#pragma once

#include "Math/MathUtils.h"
#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Core/ColorBuffer.h"

class CommandContext;
class GeometryRenderingPass;

class Edge {
public:

	enum class RootParameter {
		kColorTexture,
		kNormalTexture,
		kDepthTexture,
		kEdgeColor,
		kT,

		ParameterNum
	};

	void Initialize(ColorBuffer* originalTexture);
	void Render(CommandContext& commandContext, GeometryRenderingPass& geometryRenderingPass);

	const Vector3& GetColor() {
		return edgeColor_;
	}

	void SetColor(const Vector3& color)  {
		edgeColor_ = color;
	}

	ColorBuffer& GetResult() { return edgeTexture_; }

private:
	RootSignature rootSignature_;
	PipelineState pipelineState_;

	PipelineState multiplyPipeline_;
	RootSignature multiplyRootSignature_;

	ColorBuffer* originalTexture_;

	ColorBuffer edgeTexture_;
	Vector3 edgeColor_ = { 0.0f,0.0f,0.0f };
};