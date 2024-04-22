#include "GameScene.h"


#include "Collision/CollisionManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"
#include "Input/Input.h"
#include "Scene/SceneManager.h"
#include "GameSpeed.h"

void GameScene::OnInitialize() {
	cameraManager_ = std::make_unique<CameraManager>();
	directionalLight_ = std::make_shared<DirectionalLight>();

	editorManager_ = std::make_unique<EditorManager>();
	isMove_ = true;

	blockManager_ = std::make_unique<BlockManager>();
	fireBarManager_ = std::make_unique<FireBarManager>();
	floorManager_ = std::make_unique<FloorManager>();
	pendulumManager_ = std::make_unique<PendulumManager>();

	player_ = std::make_unique<Player>();
	boss_ = std::make_unique<Boss>();

	cameraManager_->Initialize(player_.get());

	blockManager_->SetCamera(cameraManager_->GetCamera().get());
	blockManager_->SetPlayer(player_.get());
	fireBarManager_->SetCamera(cameraManager_->GetCamera().get());
	fireBarManager_->SetPlayer(player_.get());
	floorManager_->SetCamera(cameraManager_->GetCamera().get());
	floorManager_->SetPlayer(player_.get());
	pendulumManager_->SetCamera(cameraManager_->GetCamera().get());
	pendulumManager_->SetPlayer(player_.get());

	blockManager_->Initialize(0);
	fireBarManager_->Initialize(0);
	floorManager_->Initialize(0);
	pendulumManager_->Initialize(0);

	player_->SetBoss(boss_.get());
	player_->SetStageCamera(cameraManager_->GetStageCamera());
	player_->Initialize();

	boss_->SetCamera(cameraManager_->GetCamera().get());
	boss_->Initialize();

	stageRightLight = std::make_unique<StageLineLight>();
	stageRightLight->Initialize(false);
	stageRightLight->SetPlayer(player_.get());

	stageLeftLight = std::make_unique<StageLineLight>();
	stageLeftLight->Initialize(true);
	stageLeftLight->SetPlayer(player_.get());

	stageBlockManager_ = std::make_unique<StageBlockManager>();
	stageBlockManager_->SetBoss(boss_.get());
	stageBlockManager_->Initialize();

	editorManager_->SetCamera(cameraManager_->GetCamera().get());
	skyBlockManager_ = std::make_unique<SkyBlockManager>();
	skyBlockManager_->SetBoss(boss_.get());
	skyBlockManager_->Initialize();

	editorManager_->SetPlayer(player_.get());
	editorManager_->SetBoss(boss_.get());
	editorManager_->Initialize(blockManager_.get(), fireBarManager_.get(), floorManager_.get(), pendulumManager_.get(), boss_->GetAttackTriggerManager().get());

	GameSpeed::LoadJson();
	playerDustParticle_ = std::make_unique<PlayerDustParticle>();
	playerDustParticle_->SetPlayer(player_.get());
	playerDustParticle_->Initialize();

}

void GameScene::OnUpdate() {
	directionalLight_->DrawImGui("directionalLight");

	blockManager_->Update();
	stageBlockManager_->Update();
	skyBlockManager_->Update();
	fireBarManager_->Update();
	floorManager_->Update();
	pendulumManager_->Update();
	editorManager_->Update();


	player_->Update();
	stageRightLight->Update();
	stageLeftLight->Update();
	boss_->Update();

	// 当たり判定を取る
	CollisionManager::GetInstance()->CheckCollision();

	cameraManager_->Update();
	GameSpeed::Update();

	//playerが地面にいるかの確認をするためコリジョンの下
	playerDustParticle_->Update();
#ifdef _DEBUG
	if (ImGui::Checkbox("Move", &isMove_)) {
		player_->SetIsMove(isMove_);
		boss_->SetIsMove(isMove_);
		cameraManager_->SetIsMove(isMove_);
		stageBlockManager_->SetIsMove(isMove_);
	}
	player_->SetIsMove(isMove_);
	boss_->SetIsMove(isMove_);
	cameraManager_->SetIsMove(isMove_);
	stageBlockManager_->SetIsMove(isMove_);

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
		stageBlockManager_->Reset();
		boss_->Reset(0);
		blockManager_->Reset(0);
		fireBarManager_->Reset(0);
		floorManager_->Reset(0);
		pendulumManager_->Reset(0);
		

	}
#endif // _DEBUG
	//bool changeScene = Input::GetInstance()->IsKeyTrigger(DIK_SPACE) || (Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A);
	//if (changeScene && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
	//    SceneManager::GetInstance()->ChangeScene<TitleScene>();
	//}
	RenderManager::GetInstance()->GetLightManager().Add(directionalLight_);
}

void GameScene::OnFinalize() {}
