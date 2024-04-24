#include "RenderManager.h"

#include "Core/ShaderManager.h"
#include "DefaultTextures.h"
#include "GameWindow.h"
#include "ImGuiManager.h"
#include "Math/Color.h"


#ifdef ENABLE_IMGUI
static bool useBloom = true;
static bool useEdge = true;
static bool useFog = true;
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

    //postEffect_.Initialize(preSwapChainBuffer_);

    geometryRenderingPass_.Initialize(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());
    lightingRenderingPass_.Initialize(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());

    bloom_.Initialize(&lightingRenderingPass_.GetResult());
    fxaa_.Initialize(&lightingRenderingPass_.GetResult());
    spriteRenderer_.Initialize(lightingRenderingPass_.GetResult());

    //    modelRenderer.Initialize(mainColorBuffer_, mainDepthBuffer_);
    transition_.Initialize();
    //raytracingRenderer_.Create(lightingRenderingPass_.GetResult().GetWidth(), lightingRenderingPass_.GetResult().GetHeight());

    //raymarchingRenderer_.Create(mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());

    //computeShaderTester_.Initialize(1024, 1024);
    //commandContext_.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
    //computeShaderTester_.Dispatch(commandContext_);
    //commandContext_.Finish(true);

    timer_.Initialize();

    skyTexture_.Create(L"SkyTexture", lightingRenderingPass_.GetResult().GetWidth(), lightingRenderingPass_.GetResult().GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, false);
    skyRenderer_.Initialize(skyTexture_.GetRTVFormat());
    fog_.Initialize();

    edge_.Initialize(&lightingRenderingPass_.GetResult());
    edgeMultiply_.Initialize(lightingRenderingPass_.GetResult());

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
    if (camera) {
        // 影、スペキュラ
    //    raytracingRenderer_.Render(commandContext_, *camera, *sunLight);
        geometryRenderingPass_.Render(commandContext_, *camera);
#ifdef ENABLE_IMGUI
        if (useEdge) {
#endif // ENABLE_IMGUI
            edge_.Render(commandContext_, geometryRenderingPass_);
#ifdef ENABLE_IMGUI
        }
#endif // ENABLE_IMGUI
        lightingRenderingPass_.Render(commandContext_, geometryRenderingPass_, *camera, lightManager_);
#ifdef ENABLE_IMGUI
        if (useEdge) {
#endif // ENABLE_IMGUI
            edgeMultiply_.RenderAlphaTexture(commandContext_, edge_.GetResult());
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
            if (useFog) {
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



    spriteRenderer_.Render(commandContext_, 0.0f, 0.0f, float(lightingRenderingPass_.GetResult().GetWidth()), float(lightingRenderingPass_.GetResult().GetHeight()));
    fxaa_.Render(commandContext_);

    transition_.Dispatch(commandContext_, lightingRenderingPass_.GetResult());

    auto& swapChainBuffer = swapChain_.GetColorBuffer(targetSwapChainBufferIndex);
    commandContext_.CopyBuffer(swapChainBuffer, fxaa_.GetResult());
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