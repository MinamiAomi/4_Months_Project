#include "GameScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"
#include "Scene/SceneManager.h"

#include "TitleScene.h"

void GameScene::OnInitialize() {

    debugCamera_ = std::make_shared<DebugCamera>();
    directionalLight_ = std::make_shared<DirectionalLight>();

    blockManager_ = std::make_unique<BlockManager>();
    editorManager_ = std::make_unique<EditorManager>();

    debugCamera_->Initialize();

    editorManager_->Initialize(blockManager_.get());
    blockManager_->Initialize(0);

}

void GameScene::OnUpdate() {
    directionalLight_->DrawImGui("directionalLight");
    RenderManager::GetInstance()->SetSunLight(directionalLight_);
    debugCamera_->Update();
    
    blockManager_->Update();
    editorManager_->Update();

    bool changeScene = Input::GetInstance()->IsKeyTrigger(DIK_SPACE) || (Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A);
    if (changeScene && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
        SceneManager::GetInstance()->ChangeScene<TitleScene>();
    }
}

void GameScene::OnFinalize() {
}
