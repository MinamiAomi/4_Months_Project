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
		kT,

		ParameterNum
	};

	void Initialize(ColorBuffer* originalTexture);
	void EffectRender(CommandContext& commandContext, GeometryRenderingPass& geometryRenderingPass);

	ColorBuffer& GetEffect() { return chaseEffectTexture_; }

private:
	RootSignature edgeRootSignature_;
	PipelineState edgePipelineState_;

	RootSignature effectRootSignature_;
	PipelineState effectPipelineState_;

	ColorBuffer* originalTexture_;

	ColorBuffer chaseEffectTexture_;
};