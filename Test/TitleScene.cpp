#include "TitleScene.h"

#include "Engine/Graphics/RenderManager.h"
#include "Engine/Input/Input.h"
#include "Scene/SceneManager.h"
#include "GameScene.h"
#include "Framework/ResourceManager.h"
#include "Graphics/GameWindow.h"

void TitleScene::OnInitialize() {
	camera_ = std::make_unique<Camera>();
	directionalLight_ = std::make_shared<DirectionalLight>();

	RenderManager::GetInstance()->SetCamera(camera_);

	title_ = std::make_unique<Sprite>();
	title_->SetTexture(ResourceManager::GetInstance()->FindTexture("title"));
	title_->SetIsActive(true);
	title_->SetDrawOrder(static_cast<uint8_t>(0));
	title_->SetScale({ 124.0f * 3.0f,32.0f * 3.0f });
	title_->SetRotate(0.0f);
	title_->SetPosition(Vector2{ float(GameWindow::GetInstance()->GetClientWidth() * 0.5f),float(GameWindow::GetInstance()->GetClientHeight() * 0.5f) });
	title_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	title_->SetTexcoordBase({ 0.0f,0.0f });
	title_->SetTexcoordSize({ 124.0f,32.0f });

	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("player"));
	model_->SetIsActive(true);
	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineTransform(
		{ 1.0f,1.0f,1.0f }, 
		{ Quaternion::MakeFromEulerAngle(Vector3(0.0f, 180.0f * Math::ToRadian, 0.0f)) },
		{ 0.0f,0.0f,0.0f });
	model_->SetWorldMatrix(worldMatrix);
}

void TitleScene::OnUpdate() {
	if ((Input::GetInstance()->IsKeyTrigger(DIK_SPACE) ||
		((Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
			!(Input::GetInstance()->GetPreXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A))&&
		!SceneManager::GetInstance()->GetSceneTransition().IsPlaying())
		) {
		SceneManager::GetInstance()->ChangeScene<GameScene>();
	}
	RenderManager::GetInstance()->GetLightManager().Add(directionalLight_);
}

void TitleScene::OnFinalize() {}
