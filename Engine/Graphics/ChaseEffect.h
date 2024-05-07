#pragma once

#include "Math/MathUtils.h"
#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Core/ColorBuffer.h"

class CommandContext;
class GeometryRenderingPass;

class ChaseEffect {
public:

	enum class RootParameter {
		kColorTexture,
		kNormalTexture,
		kDepthTexture,

		ParameterNum
	};

	void Initialize(ColorBuffer* originalTexture);
	void EdgeRender(CommandContext& commandContext, GeometryRenderingPass& geometryRenderingPass);
	void EffectRender(CommandContext& commandContext, GeometryRenderingPass& geometryRenderingPass);


	ColorBuffer& GetEdge() { return edgeTexture_; }
	ColorBuffer& GetEffect() { return chaseEffectTexture_; }

private:
	RootSignature edgeRootSignature_;
	PipelineState edgePipelineState_;

	RootSignature effectRootSignature_;
	PipelineState effectPipelineState_;

	ColorBuffer* originalTexture_;

	ColorBuffer edgeTexture_;

	ColorBuffer chaseEffectTexture_;
};