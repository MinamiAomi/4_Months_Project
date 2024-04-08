#include "RenderManager.h"

#include "Core/ShaderManager.h"
#include "DefaultTextures.h"
#include "GameWindow.h"
#include "ImGuiManager.h"

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
    raytracingRenderer_.Create(lightingRenderingPass_.GetResult().GetWidth(), lightingRenderingPass_.GetResult().GetHeight());

    //raymarchingRenderer_.Create(mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());

    //computeShaderTester_.Initialize(1024, 1024);
    //commandContext_.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
    //computeShaderTester_.Dispatch(commandContext_);
    //commandContext_.Finish(true);

    timer_.Initialize();

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
        lightingRenderingPass_.Render(commandContext_, geometryRenderingPass_, *camera, lightManager_);
    }

    bloom_.Render(commandContext_);
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