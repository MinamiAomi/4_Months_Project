#include "GameClearScene.h"

#include "Engine/Graphics/RenderManager.h"
#include "Engine/Input/Input.h"
#include "Scene/SceneManager.h"
#include "TitleScene.h"
#include "GameClearScene.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "Framework/ResourceManager.h"
#include "Graphics/GameWindow.h"

void GameClearScene::OnInitialize() {
	camera_ = std::make_unique<Camera>();
	directionalLight_ = std::make_shared<DirectionalLight>();

	RenderManager::GetInstance()->SetCamera(camera_);

	title_ = std::make_unique<Sprite>();
	title_->SetTexture(ResourceManager::GetInstance()->FindTexture("title"));
	title_->SetIsActive(true);
	title_->SetDrawOrder(static_cast<uint8_t>(0));
	title_->SetScale({ 864.0f,278.0f });
	title_->SetRotate(0.0f);
	title_->SetPosition(Vector2{ float(GameWindow::GetInstance()->GetClientWidth() * 0.5f),float(GameWindow::GetInstance()->GetClientHeight() * 0.5f) });
	title_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	title_->SetTexcoordBase({ 0.0f,0.0f });
	title_->SetTexcoordSize({ 864.0f,278.0f });
}

void GameClearScene::OnUpdate() {
	if ((Input::GetInstance()->IsKeyTrigger(DIK_SPACE) ||
		((Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
			!(Input::GetInstance()->GetPreXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A)))&&
		!SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
		SceneManager::GetInstance()->ChangeScene<TitleScene>(true);
	}
	RenderManager::GetInstance()->GetLightManager().Add(directionalLight_);
#ifdef _DEBUG
	// シーン変更
	if ((Input::GetInstance()->IsKeyTrigger(DIK_1) &&
		!SceneManager::GetInstance()->GetSceneTransition().IsPlaying())) {
		SceneManager::GetInstance()->ChangeScene<TitleScene>(true);
	}
	if ((Input::GetInstance()->IsKeyTrigger(DIK_2) &&
		!SceneManager::GetInstance()->GetSceneTransition().IsPlaying())
		) {
		SceneManager::GetInstance()->ChangeScene<GameScene>(true);
	}
	if ((Input::GetInstance()->IsKeyTrigger(DIK_3) &&
		!SceneManager::GetInstance()->GetSceneTransition().IsPlaying())
		) {
		SceneManager::GetInstance()->ChangeScene<GameClearScene>(true);
	}
	if ((Input::GetInstance()->IsKeyTrigger(DIK_4) &&
		!SceneManager::GetInstance()->GetSceneTransition().IsPlaying())
		) {
		SceneManager::GetInstance()->ChangeScene<GameOverScene>(true);
	}
#endif // _DEBUG

}

void GameClearScene::OnFinalize() {}
