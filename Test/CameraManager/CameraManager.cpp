#include "CameraManager.h"

#include "Externals/ImGui/imgui.h"
#include "CharacterState.h"
#include "GameSpeed.h"
#include "Boss/Boss.h"
#include "Player/Player.h"
#include "Engine/Graphics/RenderManager.h"
#include "Movie.h"
#include "Scene/SceneManager.h"

CameraManager::CameraManager()
{
	debugCamera_ = std::make_unique<DebugCamera>();
	stageCamera_ = std::make_unique<StageCamera>();
	movieCamera_ = std::make_shared<Camera>();
}

void CameraManager::Initialize(const Player* player,const Boss* boss) {

	stageCamera_->SetPlayer(player);
	stageCamera_->SetBoss(boss);
	debugCamera_->Initialize();
	stageCamera_->Initialize();

	state_ = State::kStageCamera;

	isMove_ = true;
	isDebug_ = false;
}

void CameraManager::Update() {

	//カメラ制御  これのせいでデバックカメラできません
	if (Movie::isPlaying == true) {
		RenderManager::GetInstance()->SetCamera(movieCamera_);
		state_ = kMovieCamera;
	}
	if (Movie::isPlaying == false && !SceneManager::GetInstance()->GetSceneTransition().IsPlaying()) {
		if (isDebug_ == true) {
			debugCamera_->SetRenderManager();
			state_ = kDebugCamera;
		}
		else {
			stageCamera_->SetRenderManager();
			state_ = kStageCamera;
		}
	}

#ifdef _DEBUG
	if (ImGui::BeginMenu("CameraManager")) {
		const char* items[] = { "Stage Camera", "Debug Camera" };
		static int selectedItem = static_cast<int>(state_);
		if (ImGui::Combo("State", &selectedItem, items, IM_ARRAYSIZE(items))) {
			state_ = static_cast<CameraManager::State>(selectedItem);
			switch (state_) {
			case CameraManager::kStageCamera:
			{
				stageCamera_->SetRenderManager();
				stageCamera_->transform.translate.z += distance_;
				isDebug_ = false;
			}
			break;
			case CameraManager::kDebugCamera:
			{
				debugCamera_->SetCamera(stageCamera_->GetCamera());
				debugCamera_->SetRenderManager();
				isDebug_ = true;
			}
			break;
			case CameraManager::kMovieCamera:
			{
				RenderManager::GetInstance()->SetCamera(movieCamera_);
			}
			break;
			}
		}
		ImGui::EndMenu();
	}
#endif // _DEBUG
	switch (state_) {
	case CameraManager::kStageCamera:
	{
		// 毎回するな
		stageCamera_->SetRenderManager();
		stageCamera_->Update();
		distance_ = 0.0f;
	}
	break;
	case CameraManager::kDebugCamera:
	{
		// 毎回するな
		debugCamera_->SetRenderManager();
		debugCamera_->Update();
		if (isMove_) {
			switch (Character::currentCharacterState_) {
			case Character::State::kChase:
				distance_ += GameSpeed::GetGameSpeed();
				break;
			case Character::State::kRunAway:
				distance_ -= GameSpeed::GetGameSpeed();
				break;
			default:
				break;
			}
		}
	}
	break;
	}
}

void CameraManager::Reset() {
	stageCamera_->Reset();
}

void CameraManager::SetIsMove(bool flag) {
	stageCamera_->SetIsMove(flag);
	isMove_ = flag;
}


const std::shared_ptr<Camera>& CameraManager::GetCamera() const {
	switch (state_) {
	case CameraManager::kStageCamera:
	{
		return stageCamera_->GetCamera();
	}
	break;
	case CameraManager::kDebugCamera:
	{
		return debugCamera_->GetCamera();
	}
	break;
	}
	// すべてのケースを網羅した後、デフォルトのnullポインタを返す
	static std::shared_ptr<Camera> nullCamera;
	return nullCamera;
}
