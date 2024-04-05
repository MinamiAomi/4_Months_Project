#include "CameraManager.h"

#include "Externals/ImGui/imgui.h"


void CameraManager::Initialize(Player* player) {
    player;
    debugCamera_ = std::make_unique<DebugCamera>();
    stageCamera_ = std::make_unique<StageCamera>();

    debugCamera_->Initialize();
    stageCamera_->Initialize();

    state_ = State::kStageCamera;
}

void CameraManager::Update() {
#ifdef _DEBUG
    ImGui::Begin("CameraManager");

    const char* items[] = { "Stage Camera", "Debug Camera" };
    static int selectedItem = static_cast<int>(state_);
    if (ImGui::Combo("State", &selectedItem, items, IM_ARRAYSIZE(items))) {
        state_ = static_cast<CameraManager::State>(selectedItem);
        switch (state_) {
        case CameraManager::kStageCamera:
        {
            stageCamera_->SetRenderManager();
        }
        break;
        case CameraManager::kDebugCamera:
        {
            debugCamera_->SetRenderManager();
        }
        break;
        }
    }
    ImGui::End();

#endif // _DEBUG


    switch (state_) {
    case CameraManager::kStageCamera:
    {
        stageCamera_->Update();
    }
        break;
    case CameraManager::kDebugCamera:
    {
        debugCamera_->Update();
    }
        break;
    }
}

//const std::shared_ptr<Camera>& CameraManager::GetCamera() const {
//    switch (state_) {
//    case CameraManager::kStageCamera:
//    {
//        return stageCamera_->GetCamera();
//    }
//    break;
//    case CameraManager::kDebugCamera:
//    {
//        return debugCamera_->GetCamera();
//    }
//    break;
//    }
//    // すべてのケースを網羅した後、デフォルトのnullポインタを返す
//    static std::shared_ptr<Camera> nullCamera;
//    return nullCamera;
//}
