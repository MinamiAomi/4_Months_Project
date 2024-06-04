#include "RenderManager.h"

#include "Core/ShaderManager.h"
#include "DefaultTextures.h"
#include "GameWindow.h"
#include "ImGuiManager.h"
#include "Math/Color.h"
#include "Model.h"

#include "Debug/Debug.h"

#ifdef ENABLE_IMGUI
static bool enableDebugDraw = false;
static bool useBloom = true;
static bool useEdge = true;
static bool useFog = true;
static bool enableWireFrameModel = false;
#endif // ENABLE_IMGUI


RenderManager* RenderManager::GetInstance() {
    static RenderManager instance;
    return &instance;
}

void RenderManager::Initialize() {

    graphics_ = Graphics::GetInstance();

    auto shaderManager = ShaderManager::GetInstance();
    shaderManager->Initialize();
    shaderManager->SetDirectory(std::filesystem::current_path() / SHADER_DIRECTORY);

    auto window = GameWindow::GetInstance();
    swapChain_.Create(window->GetHWND());

    DefaultTexture::Initialize();

    auto& swapChainBuffer = swapChain_.GetColorBuffer(0);
    float clearColor[4] = { 0.1f, 0.4f, 0.6f, 0.0f };
    //mainColorBuffer_.SetClearColor(clearColor);
    //mainColorBuffer_.Create(L"MainColorBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM);
    //preSwapChainBuffer_.Create(L"PreSwapChainBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM);
    //mainDepthBuffer_.Create(L"MainDepthBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_D32_FLOAT);

    temporaryScreenBuffer_.Create(L"TemporaryScreenBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), swapChainBuffer.GetRTVFormat());
    skinningManager_.Initialize();
    geometryRenderingPass_.Initialize(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());
    lightingRenderingPass_.Initialize(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());
    raytracingRenderer_.Create(lightingRenderingPass_.GetResult().GetWidth(), lightingRenderingPass_.GetResult().GetHeight());
    lineDrawer_.Initialize(lightingRenderingPass_.GetResult().GetRTVFormat());

    bloom_.Initialize(&lightingRenderingPass_.GetResult());
    fxaa_.Initialize(&lightingRenderingPass_.GetResult());
    spriteRenderer_.Initialize(temporaryScreenBuffer_);

    //    modelRenderer.Initialize(mainColorBuffer_, mainDepthBuffer_);
    transition_.Initialize();
    postEffect_.Initialize(temporaryScreenBuffer_);
    lightingPassPostEffect_.Initialize(lightingRenderingPass_.GetResult());
    //raymarchingRenderer_.Create(mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());

    //computeShaderTester_.Initialize(1024, 1024);
    //commandContext_.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
    //computeShaderTester_.Dispatch(commandContext_);
    //commandContext_.Finish(true);

    timer_.Initialize();
    wireTranslucentRenderer_.Initialize(lightingRenderingPass_.GetResult().GetRTVFormat());
    skyTexture_.Create(L"SkyTexture", lightingRenderingPass_.GetResult().GetWidth(), lightingRenderingPass_.GetResult().GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, false);
    skyRenderer_.Initialize(skyTexture_.GetRTVFormat());
    fog_.Initialize();

    edge_.Initialize(&lightingRenderingPass_.GetResult());

    chaseEffect_.Initialize(&lightingRenderingPass_.GetResult());

    whiteFilter_.Initialize(temporaryScreenBuffer_);

    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Initialize(window->GetHWND(), swapChainBuffer.GetRTVFormat());
    imguiManager->NewFrame();

    frameCount_ = 0;
}

void RenderManager::Finalize() {
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Shutdown();

    DefaultTexture::Finalize();
}

void RenderManager::Render() {

    uint32_t targetSwapChainBufferIndex = (swapChain_.GetCurrentBackBufferIndex() + 1) % SwapChain::kNumBuffers;

    auto camera = camera_.lock();

    commandContext_.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
    skinningManager_.Update(commandContext_);

    if (camera) {
        assert(!(isnan(camera->GetPosition().x) || isnan(camera->GetPosition().y) || isnan(camera->GetPosition().z)
            || isnan(camera->GetRotate().x) || isnan(camera->GetRotate().y) || isnan(camera->GetRotate().z) || isnan(camera->GetRotate().w)));




        // 影、スペキュラ
        modelSorter_.Sort(*camera);;
        // 影、スペキュラ
        assert(!lightManager_.GetDirectionalLight().empty());
        raytracingRenderer_.Render(commandContext_, *camera, lightManager_.GetDirectionalLight()[0]);


#ifdef ENABLE_IMGUI
        if (!enableDebugDraw) {
#endif // ENABLE_IMGUI
            geometryRenderingPass_.Render(commandContext_, *camera, modelSorter_);
#ifdef ENABLE_IMGUI
        }
        else {
            commandContext_.TransitionResource(geometryRenderingPass_.GetAlbedo(), D3D12_RESOURCE_STATE_RENDER_TARGET);
            commandContext_.TransitionResource(geometryRenderingPass_.GetMetallicRoughness(), D3D12_RESOURCE_STATE_RENDER_TARGET);
            commandContext_.TransitionResource(geometryRenderingPass_.GetNormal(), D3D12_RESOURCE_STATE_RENDER_TARGET);
            commandContext_.TransitionResource(geometryRenderingPass_.GetDepth(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

            commandContext_.ClearColor(geometryRenderingPass_.GetAlbedo());
            commandContext_.ClearColor(geometryRenderingPass_.GetMetallicRoughness());
            commandContext_.ClearColor(geometryRenderingPass_.GetNormal());
            commandContext_.ClearDepth(geometryRenderingPass_.GetDepth());
        }
#endif // ENABLE_IMGUI
#ifdef ENABLE_IMGUI
        if (useEdge) {
#endif // ENABLE_IMGUI
            edge_.Render(commandContext_, geometryRenderingPass_);
#ifdef ENABLE_IMGUI
        }
#endif // ENABLE_IMGUI
        lightingRenderingPass_.Render(commandContext_, geometryRenderingPass_, *camera, lightManager_);
        lightingPassPostEffect_.RenderMultiplyTexture(commandContext_, raytracingRenderer_.GetShadow());


#ifdef ENABLE_IMGUI
        if (enableWireFrameModel) {
#endif // ENABLE_IMGUI
        wireTranslucentRenderer_.Render(commandContext_, *camera, geometryRenderingPass_.GetNormal(), lightingRenderingPass_.GetResult());
#ifdef ENABLE_IMGUI
        }
#endif // ENABLE_IMGUI

#ifdef _DEBUG
        if (enableDebugDraw) {
            commandContext_.TransitionResource(lightingRenderingPass_.GetResult(), D3D12_RESOURCE_STATE_RENDER_TARGET);
            commandContext_.SetRenderTarget(lightingRenderingPass_.GetResult().GetRTV());
            commandContext_.SetViewportAndScissorRect(0, 0, lightingRenderingPass_.GetResult().GetWidth(), lightingRenderingPass_.GetResult().GetHeight());
            lineDrawer_.Render(commandContext_, *camera);
        }
        lineDrawer_.Clear();
#endif // _DEBUG


#ifdef ENABLE_IMGUI
        if (useEdge) {
#endif // ENABLE_IMGUI
            chaseEffect_.EffectRender(commandContext_, geometryRenderingPass_);
            commandContext_.CopyBuffer(lightingRenderingPass_.GetResult(), chaseEffect_.GetEffect());
            lightingPassPostEffect_.RenderAlphaTexture(commandContext_, edge_.GetResult());
#ifdef ENABLE_IMGUI
        }
#endif // ENABLE_IMGUI


        if (useSky_) {
            commandContext_.TransitionResource(skyTexture_, D3D12_RESOURCE_STATE_RENDER_TARGET);
            commandContext_.SetRenderTarget(skyTexture_.GetRTV());
            commandContext_.SetViewportAndScissorRect(0, 0, skyTexture_.GetWidth(), skyTexture_.GetHeight());
            float skyScale = camera->GetFarClip() * 2.0f;
            skyRenderer_.Render(commandContext_, *camera, Matrix4x4::MakeAffineTransform({ skyScale, skyScale, skyScale }, Quaternion::identity, camera->GetPosition()));

#ifdef ENABLE_IMGUI
            if (useFog && !enableDebugDraw) {
#endif // ENABLE_IMGUI
                fog_.Render(commandContext_, *camera, lightingRenderingPass_.GetResult(), skyTexture_, geometryRenderingPass_.GetDepth());
#ifdef ENABLE_IMGUI
            }
#endif // ENABLE_IMGUI

        }
        }



#ifdef ENABLE_IMGUI
    if (useBloom) {
#endif // ENABLE_IMGUI
        bloom_.Render(commandContext_);
#ifdef ENABLE_IMGUI
    }
#endif // ENABLE_IMGUI


    fxaa_.Render(commandContext_);

    commandContext_.TransitionResource(temporaryScreenBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.FlushResourceBarriers();
    commandContext_.SetRenderTarget(temporaryScreenBuffer_.GetRTV());
    commandContext_.SetViewportAndScissorRect(0, 0, temporaryScreenBuffer_.GetWidth(), temporaryScreenBuffer_.GetHeight());
    postEffect_.Render(commandContext_, fxaa_.GetResult());
    whiteFilter_.Render(commandContext_);

    spriteRenderer_.Render(commandContext_, 0.0f, 0.0f, float(temporaryScreenBuffer_.GetWidth()), float(temporaryScreenBuffer_.GetHeight()));

    transition_.Dispatch(commandContext_, temporaryScreenBuffer_);

    auto& swapChainBuffer = swapChain_.GetColorBuffer(targetSwapChainBufferIndex);
    commandContext_.CopyBuffer(swapChainBuffer, temporaryScreenBuffer_);
    //commandContext_.CopyBuffer(swapChainBuffer, lightingRenderingPass_.GetResult());

    commandContext_.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.FlushResourceBarriers();
    commandContext_.SetRenderTarget(swapChainBuffer.GetRTV());
    commandContext_.SetViewportAndScissorRect(0, 0, swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());


#ifdef ENABLE_IMGUI
    ImGui::Begin("Profile");
    auto io = ImGui::GetIO();
    ImGui::Text("Framerate : %f", io.Framerate);
    ImGui::Text("FrameCount : %d", frameCount_);
    ImGui::Checkbox("DebugDraw", &enableDebugDraw);
    ImGui::Checkbox("Sky", &useSky_);
    if (ImGui::TreeNode("Bloom")) {
        float knee = bloom_.GetKnee();
        float threshold = bloom_.GetThreshold();
        ImGui::Checkbox("Active", &useBloom);
        ImGui::DragFloat("knee", &knee, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("threshold", &threshold, 0.01f, 0.0f, 1.0f);
        bloom_.SetKnee(knee);
        bloom_.SetThreshold(threshold);

        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Edge")) {
        Vector3 color = edge_.GetColor();
        ImGui::Checkbox("Active", &useEdge);
        ImGui::DragFloat3("color", &color.x, 0.01f, 0.0f, 1.0f);
        edge_.SetColor(color);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Fog")) {
        float fogNear = fog_.GetNear();
        float fogFar = fog_.GetFar();
        ImGui::Checkbox("Active", &useFog);
        ImGui::DragFloat("Near", &fogNear, 1.0f, camera->GetNearClip(), camera->GetFarClip());
        ImGui::DragFloat("Far", &fogFar, 1.0f, camera->GetNearClip(), camera->GetFarClip());
        if (fogNear > fogFar) {
            std::swap(fogNear, fogFar);
        }
        fog_.SetNear(fogNear);
        fog_.SetFar(fogFar);
        ImGui::TreePop();
    }
    ImGui::Checkbox("WireFrameModel", &enableWireFrameModel);
    ImGui::End();
#endif // ENABLE_IMGUI

    // ImGuiを描画
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Render(commandContext_);


    commandContext_.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_PRESENT);

    // コマンドリスト完成(クローズ)
    commandContext_.Close();

    // バックバッファをフリップ
    swapChain_.Present();
    frameCount_++;
    // シグナルを発行し待つ
    auto& commandQueue = graphics_->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    commandQueue.WaitForIdle();

    commandContext_.Finish(false);

    graphics_->GetReleasedObjectTracker().FrameIncrementForRelease();
    timer_.KeepFrameRate(60);


    imguiManager->NewFrame();
    // ライトをリセット
    lightManager_.Reset();
}