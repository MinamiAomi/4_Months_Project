#include "GameOverScene.h"

#include "Engine/Graphics/RenderManager.h"
#include "Engine/Input/Input.h"
#include "Scene/SceneManager.h"
#include "TitleScene.h"
#include "GameClearScene.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "Framework/ResourceManager.h"
#include "Graphics/GameWindow.h"
#include "Graphics/ImGuiManager.h"

void GameOverScene::OnInitialize() {
	SkyRenderer::SetZero();
	camera_ = std::make_unique<Camera>();
	directionalLight_ = std::make_shared<DirectionalLight>();

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();

	camera_->SetPosition({ 0.0f,25.0f,-20.0f });
	camera_->UpdateMatrices();
	RenderManager::GetInstance()->SetCamera(camera_);

	title_ = std::make_unique<Sprite>();
	title_->SetTexture(ResourceManager::GetInstance()->FindTexture("gameOver"));
	title_->SetIsActive(true);
	title_->SetDrawOrder(static_cast<uint8_t>(0));
	title_->SetScale({ 739.0f,139.0f });
	title_->SetRotate(0.0f);
	title_->SetPosition(Vector2{ float(GameWindow::GetInstance()->GetClientWidth() * 0.5f),float(GameWindow::GetInstance()->GetClientHeight() * 0.25f) });
	title_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	title_->SetTexcoordBase({ 0.0f,0.0f });
	title_->SetTexcoordSize({ 739.0f,139.0f});

	gameOverBoss_ = std::make_unique<GameOverBoss>();
	gameOverBoss_->Initialize();
}

void GameOverScene::OnUpdate() {
	gameOverBoss_->Update();
	if ((Input::GetInstance()->IsKeyTrigger(DIK_SPACE) ||
		((Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
			!(Input::GetInstance()->GetPreXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A)))&&
		!SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
		SceneManager::GetInstance()->ChangeScene<TitleScene>(true);
	}
	RenderManager::GetInstance()->GetLightManager().Add(directionalLight_);
#ifdef _DEBUG
	// シーン変更
	if ((Input::GetInstance()->IsKeyTrigger(DIK_U) &&
		!SceneManager::GetInstance()->GetSceneTransition().IsPlaying())) {
		SceneManager::GetInstance()->ChangeScene<TitleScene>(true);
	}
	if ((Input::GetInstance()->IsKeyTrigger(DIK_I) &&
		!SceneManager::GetInstance()->GetSceneTransition().IsPlaying())
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
	if (ImGui::Checkbox("DebugCamera", &isDebugCamera_)) {

		if (isDebugCamera_) {
			RenderManager::GetInstance()->SetCamera(debugCamera_->GetCamera());
		}
		else {
			RenderManager::GetInstance()->SetCamera(camera_);
		}
	}
	if (isDebugCamera_) {
		debugCamera_->Update();
	}
#endif // _DEBUG
}

void GameOverScene::OnFinalize() {}
