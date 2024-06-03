#include "GameScene.h"


#include "Collision/CollisionManager.h"
#include "GameSpeed.h"
#include "GameClearScene.h"
#include "GameOverScene.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"
#include "Input/Input.h"
#include "Scene/SceneManager.h"
#include "Framework/ResourceManager.h"
#include "TitleScene.h"
#include "Debug/Debug.h"

#include "WindManager.h"

void GameScene::OnInitialize() {

	cameraManager_ = std::make_unique<CameraManager>();
	directionalLight_ = std::make_shared<DirectionalLight>();
	directionalLight_->direction = Vector3(0.1f, -1.0f, 0.3f).Normalized();

	stageLoop_ = std::make_unique<StageLoop>();

	//editorManager_ = std::make_unique<EditorManager>();
	isMove_ = true;

	player_ = std::make_unique<Player>();
	boss_ = std::make_unique<Boss>();

	//player_->SetTrapManager(stageLoop_->GetTrapManager().get());
	player_->SetBoss(boss_.get());
	player_->SetCamera(cameraManager_->GetCamera().get());
	player_->Initialize();

	cameraManager_->Initialize(player_.get(), boss_.get());
	startMovie_ = std::make_unique<StartMovie>();
	startMovie_->Initialize(player_.get(), boss_.get(), cameraManager_->GetMovieCamera(), cameraManager_->GetStageCamera());
	gameClearMovie_ = std::make_unique<GameClearMovie>();
	gameClearMovie_->Initialize(player_.get(), boss_.get(), cameraManager_->GetMovieCamera(), cameraManager_->GetStageCamera());
	gameOverMovie_ = std::make_unique<GameOverMovie>();
	gameOverMovie_->Initialize(player_.get(), boss_.get(), cameraManager_->GetMovieCamera(), cameraManager_->GetStageCamera());
	gameStartMovie_ = std::make_unique<GameStartMovie>();
	gameStartMovie_->Initialize(player_.get(), boss_.get(), cameraManager_->GetMovieCamera(), cameraManager_->GetStageCamera());
	hammerMovie_ = std::make_unique<HammerMovie>();
	hammerMovie_->Initialize(player_.get(), boss_.get(), cameraManager_->GetMovieCamera(), cameraManager_->GetStageCamera());

	currentMovie_ = startMovie_.get();
	Movie::isPlaying = true;

	boss_->SetPlayer(player_.get());
	boss_->SetCamera(cameraManager_->GetCamera().get());
	boss_->Initialize();

	stageLoop_->SetPlayer(player_.get());
	stageLoop_->SetBoss(boss_.get());
	stageLoop_->SetCamera(cameraManager_->GetCamera().get());
	stageLoop_->SetHammerMovie(hammerMovie_.get());
	stageLoop_->Initialize();

	stageBlockManager_ = std::make_unique<StageBlockManager>();
	stageBlockManager_->SetBoss(boss_.get());
	stageBlockManager_->Initialize();


	for (std::unique_ptr<StageLineLight>& stageLineLight : stageLineLights_) {
		stageLineLight = std::make_unique<StageLineLight>();
		stageLineLight->SetPlayer(player_.get());
	}

	stageLineLights_[0]->Initialize(false, false);
	stageLineLights_[1]->Initialize(true, false);
	stageLineLights_[2]->Initialize(true, true);
	stageLineLights_[3]->Initialize(false, true);

	for (std::unique_ptr<StageLineLight>& stageLineLight : stageLineLights_) {
		stageLineLight->Update();
	}

	//editorManager_->SetCamera(cameraManager_->GetCamera().get());
	skyBlockManager_ = std::make_unique<SkyBlockManager>();
	skyBlockManager_->SetBoss(boss_.get());
	skyBlockManager_->Initialize();

	//editorManager_->SetPlayer(player_.get());
	//editorManager_->SetBoss(boss_.get());
	//editorManager_->Initialize(stageLoop_->GetBlockManager().get(), stageLoop_->GetFireBarManager().get(), stageLoop_->GetFloorManager().get(), stageLoop_->GetPendulumManager().get(), stageLoop_->GetBossAttackTriggerManager().get());

	playerDustParticle_ = std::make_unique<PlayerDustParticle>();
	playerDustParticle_->SetPlayer(player_.get());
	playerDustParticle_->Initialize();

	ui_ = std::make_unique<UI>();
	ui_->Initialize();

	cutIn_ = std::make_unique<CutIn>();
	cutIn_->Initialize();

	GameSpeed::LoadJson();
	Character::LoadJson();
	// ikkaideke
	Initialize();

	chaseBGM_ = ResourceManager::GetInstance()->FindSound("chaseBGM");
	revengeBGM_ = ResourceManager::GetInstance()->FindSound("revengeBGM");
	bgm_ = revengeBGM_;
	bgm_.SetVolume(0.1f);
	bgm_.Play(true);

	pause_ = std::make_unique<Pause>();
	pause_->Initialize();

	gameStartMovie_->ResetIsEnd();

	Character::isEndFirstChange_ = false;

	Movie::isClearGameOver = false;
}

void GameScene::OnUpdate() {

	//ライティングされなくなるからこれだけ
	for (std::unique_ptr<StageLineLight>& stageLineLight : stageLineLights_) {
		stageLineLight->Update();
	}

	pause_->Update();
	ui_->Update();

	if (!pause_->GetIsPause()) {

		//gameClear
		if ((!boss_->GetIsAlive() && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying() && !Movie::isPlaying) || Input::GetInstance()->IsKeyTrigger(DIK_C)) {
			Movie::isPlaying = true;
			currentMovie_ = gameClearMovie_.get();
		}
		//gameOver
		if ((!player_->GetIsAlive() && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying() && !Movie::isPlaying) || Input::GetInstance()->IsKeyTrigger(DIK_K)) {
			Movie::isPlaying = true;
			currentMovie_ = gameOverMovie_.get();
		}
		//hammer
		if ((boss_->GetIsHit() && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying() && !Movie::isPlaying) || Input::GetInstance()->IsKeyTrigger(DIK_F)) {
			Movie::isPlaying = true;
			currentMovie_ = hammerMovie_.get();
		}

		Movie::SetFalseIsEndFrame();

		if (currentMovie_) {
			currentMovie_->Update();
			if (Movie::isPlaying == false) {
				currentMovie_->Reset();
				currentMovie_ = nullptr;
				if (!player_->GetIsAlive()) {
					SceneManager::GetInstance()->ChangeScene<GameOverScene>(true);
				}
				if (!boss_->GetIsAlive()) {
					SceneManager::GetInstance()->ChangeScene<GameClearScene>(true);
				}
			}
		}

		//gameStart
		if ((boss_->GetIsFirstHit() && !gameStartMovie_->GetIsEnd() && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying() && !Movie::isPlaying) || Input::GetInstance()->IsKeyTrigger(DIK_F)) {
			Movie::isPlaying = true;
			currentMovie_ = gameStartMovie_.get();
		}


		if (!SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {

			directionalLight_->DrawImGui("directionalLight");

			if (Character::currentCharacterState_ == Character::State::kScneChange) {
				switch (Character::nextCharacterState_) {
				case Character::State::kChase:
				{
					bgm_.Stop();
					bgm_ = revengeBGM_;
					bgm_.Play(true);
					bgm_.SetVolume(0.1f);
					break;
				}
				case Character::State::kRunAway:
				{
					bgm_.Stop();
					bgm_ = chaseBGM_;
					bgm_.Play(true);
					bgm_.SetVolume(0.1f);
					break;
				}
				default:
					break;
				}
			}

			stageLoop_->Update();

			

			//ムービー中動いてほしくないもの
			if (!Movie::isPlaying && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
				player_->Update();
				boss_->Update();
				stageBlockManager_->Update();
				WindManager::GetInstance()->Update();
			}
			player_->SceneChangeUpdate();
			boss_->MovieUpdate();

			skyBlockManager_->Update();

			cutIn_->Update();

			// 当たり判定を取る
			auto time = Debug::ElapsedTime([&]() {
				CollisionManager::GetInstance()->CheckCollision();
				});
			Debug::Log(std::format("Collision : {}\n", time));

			cameraManager_->Update();
			GameSpeed::Update();
			Character::Update();



			//playerが地面にいるかの確認をするためコリジョンの下(いいコメントアウトだね＾＾)
			playerDustParticle_->Update();
#ifdef _DEBUG
			//editorManager_->Update();
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
				static int selectedItem = static_cast<int>(Character::currentCharacterState_);
				if (ImGui::Combo("State", &selectedItem, items, IM_ARRAYSIZE(items))) {
					Character::currentCharacterState_ = static_cast<Character::State>(selectedItem);
					switch (Character::currentCharacterState_) {
					case Character::State::kChase:
					{
						Character::SetNextScene(Character::State::kChase);
					}
					break;
					case Character::State::kRunAway:
					{
						Character::SetNextScene(Character::State::kRunAway);
					}
					break;
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::Button("Reset") ||
				Input::GetInstance()->IsKeyTrigger(DIK_R)) {
				player_->Reset();
				cameraManager_->Reset();
				boss_->Reset(0);
				stageBlockManager_->Reset();
				skyBlockManager_->Reset();
				stageLoop_->Reset();
				gameStartMovie_->ResetIsEnd();
			}
			// シーン変更
			if ((Input::GetInstance()->IsKeyTrigger(DIK_U) &&
				!SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) ||
				pause_->GetOrderToTitle()) {
				SceneManager::GetInstance()->ChangeScene<TitleScene>(true);
			}
			if ((Input::GetInstance()->IsKeyTrigger(DIK_I) &&
				!SceneManager::GetInstance()->GetSceneTransition().IsPlaying()||
				pause_->GetOrderReset())
				) {
				SceneManager::GetInstance()->ChangeScene<GameScene>(true);
			}
			if ((Input::GetInstance()->IsKeyTrigger(DIK_O) &&
				!SceneManager::GetInstance()->GetSceneTransition().IsPlaying())
				) {
				SceneManager::GetInstance()->ChangeScene<GameClearScene>(true);
			}
			if ((Input::GetInstance()->IsKeyTrigger(DIK_P) &&
				!SceneManager::GetInstance()->GetSceneTransition().IsPlaying())
				) {
				SceneManager::GetInstance()->ChangeScene<GameOverScene>(true);
			}
#endif // _DEBUG
			if (Input::GetInstance()->IsKeyTrigger(DIK_R)) {
				player_->Reset();
				cameraManager_->Reset();
				stageBlockManager_->Reset();
				boss_->Reset(0);
				stageLoop_->Reset();
			}
			//if (!player_->GetIsAlive() && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
			//    SceneManager::GetInstance()->ChangeScene<GameOverScene>(true);
			//}
			//if (!boss_->GetIsAlive() && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
			//    SceneManager::GetInstance()->ChangeScene<GameClearScene>(true);
			//}
		}
	}

	RenderManager::GetInstance()->GetLightManager().Add(directionalLight_);
}

void GameScene::OnFinalize() {}

void GameScene::Initialize() {

	directionalLight_->DrawImGui("directionalLight");

	stageLoop_->Update();
	
	stageBlockManager_->Update();
	
	player_->Update();
	//ライティングされなくなるからこれだけ
	for (std::unique_ptr<StageLineLight>& stageLineLight : stageLineLights_) {
		stageLineLight->Update();
	}
	boss_->Update();

	skyBlockManager_->Update();
	ui_->Update();
	cutIn_->Update();
	// 当たり判定を取る
	CollisionManager::GetInstance()->CheckCollision();

	cameraManager_->Update();
	GameSpeed::Update();
	Character::Update();

	//playerが地面にいるかの確認をするためコリジョンの下(いいコメントアウトだね＾＾)
	playerDustParticle_->Update();

	RenderManager::GetInstance()->GetLightManager().Add(directionalLight_);
}
