#include "GameScene.h"


#include "Graphics/ImGuiManager.h"
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
	
	RenderManager::GetInstance()->SetSunLight(directionalLight_);

	cameraManager_->Initialize(player_.get());

	editorManager_->Initialize(blockManager_.get());
	blockManager_->Initialize(0);

	player_->Initialize();

	for (uint32_t i = 0; auto & floor : floor_) {
		floor = std::make_unique<Floor>();
		floor->Initialize();
		floor->SetLocalPos({ 0.0f,-2.0f , -floor->GetZLength() / 2.0f + i * floor->GetZLength()});
		i++;
	}
}

void GameScene::OnUpdate() {
	directionalLight_->DrawImGui("directionalLight");

	blockManager_->Update();
	editorManager_->Update();

	player_->Update();
	for (int i = 0; auto & floor : floor_) {
		floor->Update();
		int playerNum = static_cast<int>(player_->transform.translate.z / floor->GetZLength());
		float playerLocation = std::fmodf(player_->transform.translate.z , floor->GetZLength());
		//playerがのっていない
		if (std::abs(playerNum % 2) != i) {
			//playerが半分より-であれば
			if (playerLocation < (-floor->GetZLength() / 2.0f)) {
				floor->transform.translate.z = (playerNum - 1) * floor->GetZLength() - floor->GetZLength() / 2.0f;
			}
		}
		i++;
	}
	cameraManager_->Update();
	if (ImGui::Button("Reset")) {
		player_->Reset();
		cameraManager_->Reset();
	}
	//bool changeScene = Input::GetInstance()->IsKeyTrigger(DIK_SPACE) || (Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A);
	//if (changeScene && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
	//    SceneManager::GetInstance()->ChangeScene<TitleScene>();
	//}
}

void GameScene::OnFinalize() {}
