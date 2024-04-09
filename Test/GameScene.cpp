#include "GameScene.h"


#include "Collision/CollisionManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"
#include "Input/Input.h"
#include "Scene/SceneManager.h"

void GameScene::OnInitialize() {
	cameraManager_ = std::make_unique<CameraManager>();
	directionalLight_ = std::make_shared<DirectionalLight>();

	editorManager_ = std::make_unique<EditorManager>();
	isMove_ = true;

	blockManager_ = std::make_unique<BlockManager>();
	fireBarManager_ = std::make_unique<FireBarManager>();

	player_ = std::make_unique<Player>();
	boss_ = std::make_unique<Boss>();

	RenderManager::GetInstance()->SetSunLight(directionalLight_);

	cameraManager_->Initialize(player_.get());

	editorManager_->Initialize(blockManager_.get(), fireBarManager_.get());
	
	blockManager_->SetPlayer(player_.get());
	blockManager_->Initialize(0);
	fireBarManager_->SetPlayer(player_.get());
	fireBarManager_->Initialize(0);

	player_->SetBoss(boss_.get());
	player_->SetStageCamera(cameraManager_->GetStageCamera());
	player_->Initialize();

	boss_->Initialize();
	for (uint32_t i = 0; auto & floor : floor_) {
		floor = std::make_unique<Floor>();
		floor->Initialize();
		floor->SetLocalPos({ 0.0f, 0.0f , -floor->GetZLength() / 2.0f + i * floor->GetZLength() });
		i++;
	}
}

void GameScene::OnUpdate() {
	directionalLight_->DrawImGui("directionalLight");

	blockManager_->Update();
	fireBarManager_->Update();
	editorManager_->Update();

	player_->Update();
	boss_->Update();

	for (int i = 0; auto & floor : floor_) {
		floor->Update();
		int playerNum = static_cast<int>(player_->transform.translate.z / floor->GetZLength());
		float playerLocation = std::fmodf(player_->transform.translate.z, floor->GetZLength());
		//playerがのっていない
		if (std::abs(playerNum % 2) != i) {
			//playerが半分より-であれば
			if (playerLocation < (-floor->GetZLength() / 2.0f)) {
				floor->transform.translate.z = (playerNum - 1) * floor->GetZLength() - floor->GetZLength() / 2.0f;
			}
		}
		i++;
	}

	// 当たり判定を取る
	CollisionManager::GetInstance()->CheckCollision();

	cameraManager_->Update();
#ifdef _DEBUG
	/*if (Input::GetInstance()->IsKeyTrigger(DIK_TAB)) {
		if (isMove_) {
			cameraManager_->SetState(CameraManager::State::kDebugCamera);
			isMove_ = false;
		}
		else {
			cameraManager_->SetState(CameraManager::State::kStageCamera);
			isMove_ = true;
		}
	}*/
	if (ImGui::Checkbox("Move", &isMove_)) {
		player_->SetIsMove(isMove_);
		boss_->SetIsMove(isMove_);
		cameraManager_->SetIsMove(isMove_);
	}
	player_->SetIsMove(isMove_);
	boss_->SetIsMove(isMove_);
	cameraManager_->SetIsMove(isMove_);
	if (ImGui::BeginMenu("CharacterState")) {
		const char* items[] = { "Chase", "RunAway" };
		static int selectedItem = static_cast<int>(characterState_);
		if (ImGui::Combo("State", &selectedItem, items, IM_ARRAYSIZE(items))) {
			characterState_ = static_cast<Character::State>(selectedItem);
			switch (characterState_) {
			case Character::State::kChase:
			{
				characterState_ = Character::State::kChase;
			}
			break;
			case Character::State::kRunAway:
			{
				characterState_ = Character::State::kRunAway;
			}
			break;
			}
		}
		ImGui::EndMenu();
	}
	if (ImGui::Button("Reset")) {
		player_->Reset();
		cameraManager_->Reset();
		boss_->Reset();
		blockManager_->Reset(0);
		fireBarManager_->Reset(0);
		for (uint32_t i = 0; auto & floor : floor_) {
			floor->SetLocalPos({ 0.0f,0.0f , -floor->GetZLength() / 2.0f + i * floor->GetZLength() });
			i++;
		}
	}
#endif // _DEBUG
	//bool changeScene = Input::GetInstance()->IsKeyTrigger(DIK_SPACE) || (Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A);
	//if (changeScene && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
	//    SceneManager::GetInstance()->ChangeScene<TitleScene>();
	//}
}

void GameScene::OnFinalize() {}
