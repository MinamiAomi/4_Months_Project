#include "CameraManager.h"

#include "Externals/ImGui/imgui.h"
#include "CharacterState.h"
#include "GameSpeed.h"

void CameraManager::Initialize(Player* player) {
	player;
	debugCamera_ = std::make_unique<DebugCamera>();
	stageCamera_ = std::make_unique<StageCamera>();

	debugCamera_->Initialize();
	stageCamera_->Initialize();

	state_ = State::kStageCamera;

	isMove_ = true;
}

void CameraManager::Update() {
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
			}
			break;
			case CameraManager::kDebugCamera:
			{
				debugCamera_->SetCamera(stageCamera_->GetCamera());
				debugCamera_->SetRenderManager();
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
			if (characterState_ == Character::kRunAway) {
				distance_ -= GameSpeed::speed;
			}
			else {
				distance_ += GameSpeed::speed;
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
