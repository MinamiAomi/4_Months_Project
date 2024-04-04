#include "GameScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"
#include "Scene/SceneManager.h"

#include "TitleScene.h"

void GameScene::OnInitialize() {
    cameraManager_ = std::make_unique<CameraManager>();
    directionalLight_ = std::make_shared<DirectionalLight>();

    blockManager_ = std::make_unique<BlockManager>();
    editorManager_ = std::make_unique<EditorManager>();

    player_ = std::make_unique<Player>();

    floor_ = std::make_unique<Floor>();

    cameraManager_->Initialize(player_.get());

    editorManager_->Initialize(blockManager_.get());
    blockManager_->Initialize(0);

    player_->Initialize();

    floor_->Initialize();
}

void GameScene::OnUpdate() {
    directionalLight_->DrawImGui("directionalLight");
    RenderManager::GetInstance()->SetSunLight(directionalLight_);
    
    blockManager_->Update();
    editorManager_->Update();

    player_->Update();
    floor_->Update();
    cameraManager_->Update();
    //bool changeScene = Input::GetInstance()->IsKeyTrigger(DIK_SPACE) || (Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A);
    //if (changeScene && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
    //    SceneManager::GetInstance()->ChangeScene<TitleScene>();
    //}
}

void GameScene::OnFinalize() {
}
