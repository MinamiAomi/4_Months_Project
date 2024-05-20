#pragma once

#include "Math/MathUtils.h"
#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Core/ColorBuffer.h"

class CommandContext;
class GeometryRenderingPass;

class WhiteFilter {
public:

	enum class RootParameter {
		kT,

		ParameterNum
	};

	void Initialize(ColorBuffer& originalTexture);
	void Render(CommandContext& commandContext);

	ColorBuffer& GetEffect() { return whiteFilterTexture_; }

private:
	RootSignature rootSignature_;
	PipelineState pipelineState_;

	ColorBuffer* originalTexture_;

	ColorBuffer whiteFilterTexture_;
};