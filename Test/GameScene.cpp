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


	cameraManager_->Initialize(player_.get());

	editorManager_->Initialize(blockManager_.get());
	blockManager_->Initialize(0);

	player_->Initialize();

	for (uint32_t i = 0; auto & floor : floor_) {
		floor = std::make_unique<Floor>();
		floor->Initialize();
		floor->SetLocalPos({ 0.0f,0.0f , i * -550.0f});
		i++;
	}
}

void GameScene::OnUpdate() {
	directionalLight_->DrawImGui("directionalLight");
	RenderManager::GetInstance()->SetSunLight(directionalLight_);

	blockManager_->Update();
	editorManager_->Update();

	player_->Update();
	for (auto& floor : floor_) {
		floor->Update();
		//auto tmp = player_->GetLocalPos().z - floor->GetLocalPos().z;
		//if (std::fabs(tmp) > 550.0f * 0.4f) {
		//	auto& pos = floor->GetLocalPos();
		//	if (player_->GetLocalPos().z - floor->GetLocalPos().z < 0.0f) {
		//		floor->SetLocalPos({ pos.x,pos.y,pos.z - 550.0f * 0.5f });

		//	}
		//	else {
		//		floor->SetLocalPos({ pos.x,pos.y,pos.z + 550.0f * 0.5f });

		//	}
		//}
	}
	cameraManager_->Update();
	//bool changeScene = Input::GetInstance()->IsKeyTrigger(DIK_SPACE) || (Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A);
	//if (changeScene && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
	//    SceneManager::GetInstance()->ChangeScene<TitleScene>();
	//}
}

void GameScene::OnFinalize() {}
