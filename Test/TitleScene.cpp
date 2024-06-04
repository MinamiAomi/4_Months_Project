#include "TitleScene.h"

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

void TitleScene::OnInitialize() {
	camera_ = std::make_shared<Camera>();
	directionalLight_ = std::make_shared<DirectionalLight>();

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();

	// ここ手打ちで
	camera_->SetRotate(Quaternion::MakeFromEulerAngle({ -0.08f,-0.3f,0.0f }));
	camera_->SetPosition({ 14.8f,3.02f,-18.7f });
	camera_->UpdateMatrices();
	RenderManager::GetInstance()->SetCamera(camera_);

	title_ = std::make_unique<Sprite>();
	title_->SetTexture(ResourceManager::GetInstance()->FindTexture("title"));
	title_->SetIsActive(true);
	title_->SetDrawOrder(static_cast<uint8_t>(0));
	title_->SetScale({ 864.0f*0.8f,278.0f * 0.8f });
	title_->SetRotate(0.0f);
	title_->SetPosition(Vector2{ float(1920.0f * 0.25f),float(1080.0f * 0.7f) });
	title_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	title_->SetTexcoordBase({ 0.0f,0.0f });
	title_->SetTexcoordSize({ 864.0f,278.0f });

	UI_ = std::make_unique<TitleUI>();
	UI_->Initialize();

	titlePlayer_ = std::make_unique<TitlePlayer>();
	titlePlayer_->Initialize();

	for (int i = 0; i < 3; ++i) {
		titleFloor_.at(i) = std::make_unique<TitleFloor>();
		titleFloor_.at(i)->Initialize();
		float modelSize = titleFloor_.at(i)->GetModel()->GetModel()->GetMeshes().at(0).maxVertex.x - titleFloor_.at(i)->GetModel()->GetModel()->GetMeshes().at(0).minVertex.x;
		titleFloor_.at(i)->transform.translate.z = modelSize * (i - 1);
	}

	titleBoss_ = std::make_unique<TitleBoss>();
	titleBoss_->Initialize();
}

void TitleScene::OnUpdate() {

	titlePlayer_->Update();
	for (uint32_t i = 0; i < 3; ++i) {
		titleFloor_.at(i)->transform.translate.z += 0.5f;
		titleFloor_.at(i)->Update();
		float modelSize = titleFloor_.at(i)->GetModel()->GetModel()->GetMeshes().at(0).maxVertex.x - titleFloor_.at(i)->GetModel()->GetModel()->GetMeshes().at(0).minVertex.x;
		if (titleFloor_.at(i)->transform.translate.z > modelSize * 2.0f) {
			titleFloor_.at(i)->transform.translate.z = -modelSize;
		}
	}
	titleBoss_->Update();
	UI_->Update();
	if (((Input::GetInstance()->IsKeyTrigger(DIK_A)) ||
		(Input::GetInstance()->IsKeyTrigger(DIK_SPACE) ||
			((Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
				!(Input::GetInstance()->GetPreXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A))) &&
		!SceneManager::GetInstance()->GetSceneTransition().IsPlaying())
		) {
		SceneManager::GetInstance()->ChangeScene<GameScene>(true);
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

void TitleScene::OnFinalize() {}
