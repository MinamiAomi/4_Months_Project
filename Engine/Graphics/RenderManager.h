#pragma once

#include "Math/Camera.h"
#include "Core/Graphics.h"
#include "Core/SwapChain.h"
#include "Core/CommandContext.h"
#include "Core/ColorBuffer.h"
#include "Core/DepthBuffer.h"
#include "Raytracing/RaytracingRenderer.h"
#include "RaymarchingRenderer.h"
#include "Bloom.h"
#include "FXAA.h"
#include "ParticleRenderer.h"
#include "SpriteRenderer.h"
#include "PostEffect.h"
#include "Timer.h"
#include "LightManager.h"
#include "ComputeShader.h"
#include "Transition.h"
#include "ModelSorter.h"
#include "SkinningManager.h"
#include "GeometryRenderingPass.h"
#include "LightingRenderingPass.h"
#include "LineDrawer.h"
#include "App/SkyRenderer.h"
#include "App/Fog.h"
#include "App/ForemostRenderer.h"
#include "Edge.h"
#include "ChaseEffect.h"
#include "WhiteFilter.h"

#ifdef _DEBUG
#define SHADER_DIRECTORY "../Engine/Graphics/Shader"
#else
#define SHADER_DIRECTORY "Resources/Shader"
#endif // _DEBUG

class RenderManager {
public:

    static RenderManager* GetInstance();

    void Initialize();
    void Finalize();
    void Render();

    void SetCamera(const std::shared_ptr<Camera>& camera) { camera_ = camera; }
    void SetSkyWorldMatrix(const Matrix4x4& mat) { skyTransform_ = mat; }
    void SetUseSky(bool useSky) { useSky_ = useSky; }
    LightManager& GetLightManager() { return lightManager_; }
    Bloom& GetBloom() { return bloom_; }
    Transition& GetTransition() { return transition_; }  
    SkinningManager& GetSkinningManager() { return skinningManager_; }
    LineDrawer& GetLineDrawer() { return lineDrawer_; }


private:
    RenderManager() = default;
    RenderManager(const RenderManager&) = delete;
    RenderManager& operator=(const RenderManager&) = delete;

    Graphics* graphics_ = nullptr;
    SwapChain swapChain_;
    CommandContext commandContext_;

    ColorBuffer temporaryScreenBuffer_;

    SpriteRenderer spriteRenderer_;

    ModelSorter modelSorter_;
    SkinningManager skinningManager_;
    GeometryRenderingPass geometryRenderingPass_;
    LightingRenderingPass lightingRenderingPass_;
    LineDrawer lineDrawer_;

    RaytracingRenderer raytracingRenderer_;
    //RaymarchingRenderer raymarchingRenderer_;
    //ModelRenderer modelRenderer;
    Transition transition_;
    Bloom bloom_;
    Edge edge_;
    ChaseEffect chaseEffect_;
    FXAA fxaa_;
    PostEffect postEffect_;
    PostEffect lightingPassPostEffect_;

    WhiteFilter whiteFilter_;

    ColorBuffer skyTexture_;
    SkyRenderer skyRenderer_;
    Fog fog_;
    ForemostRenderer foremostRenderer_;
    Matrix4x4 skyTransform_;

    Timer timer_;
    std::weak_ptr<const Camera> camera_;
    LightManager lightManager_;

    UINT64 frameCount_;
    bool raymarching_ = false;
    bool useSky_ = true;
};