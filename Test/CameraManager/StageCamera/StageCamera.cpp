#include "StageCamera.h"

#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"
#include "File/JsonHelper.h"
#include "Math/MathUtils.h"
#include "Input/Input.h"
#include "Player/Player.h"
#include "GameSpeed.h"

void StageCamera::Initialize() {
	camera_ = std::make_shared<Camera>();
	RenderManager::GetInstance()->SetCamera(camera_);

	transform.rotate = Quaternion::MakeForXAxis(10.0f * Math::ToRadian);

	isMove_ = true;
#pragma region パラメーター
	JSON_OPEN("Resources/Data/StageCamera/StageCamera.json");
	for (uint32_t i = 0; i < cameraParam_.size(); i++) {
		JSON_OBJECT("cameraParam_" + std::to_string(i));
		cameraParam_.at(i).Load();
		JSON_ROOT();
	}
	JSON_CLOSE();
#pragma endregion
}

void StageCamera::Update() {

	switch (characterState_) {
	case Character::State::kChase:
	{
		if (isMove_) {
			//transform.translate += cameraParam_.at(Character::State::kChase).cameraVelocity;
			transform.translate.z += GameSpeed::speed;
		}
		camera_->SetPosition(
			{
			transform.translate.x + cameraParam_.at(Character::State::kChase).offset.x,
			transform.translate.y + cameraParam_.at(Character::State::kChase).offset.y,
			transform.translate.z + cameraParam_.at(Character::State::kChase).offset.z
			});
		camera_->SetRotate(Quaternion::MakeFromEulerAngle(cameraParam_.at(Character::State::kChase).eulerAngle * Math::ToRadian));
	}
	break;
	case Character::State::kRunAway:
	{
		if (isMove_) {
			//transform.translate -= cameraParam_.at(Character::State::kRunAway).cameraVelocity;
			transform.translate.z -= GameSpeed::speed;
		}
		camera_->SetPosition(
			{
			transform.translate.x + cameraParam_.at(Character::State::kRunAway).offset.x,
			transform.translate.y + cameraParam_.at(Character::State::kRunAway).offset.y,
			transform.translate.z + cameraParam_.at(Character::State::kRunAway).offset.z
			});
		camera_->SetRotate(Quaternion::MakeFromEulerAngle(cameraParam_.at(Character::State::kRunAway).eulerAngle * Math::ToRadian));
	}
	break;
	default:
		break;
	}
	camera_->UpdateMatrices();
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("CameraManager")) {
		ImGui::DragFloat3("Pos", &transform.translate.x, 0.1f);
		switch (characterState_) {
		case Character::State::kChase:
		{
			ImGui::DragFloat3("cameraVelocity", &cameraParam_.at(Character::State::kChase).cameraVelocity.x, 0.1f);
			Vector3 pos = transform.translate - cameraParam_.at(Character::State::kChase).offset;
			ImGui::DragFloat3("offset", &cameraParam_.at(Character::State::kChase).offset.x, 0.1f);
			transform.translate = pos + cameraParam_.at(Character::State::kChase).offset;
			ImGui::DragFloat3("eulerAngle", &cameraParam_.at(Character::State::kChase).eulerAngle.x, 0.1f);

		}
		break;
		case Character::State::kRunAway:
		{
			ImGui::DragFloat3("cameraVelocity", &cameraParam_.at(Character::State::kRunAway).cameraVelocity.x, 0.1f);
			Vector3 pos = transform.translate - cameraParam_.at(Character::State::kRunAway).offset;
			ImGui::DragFloat3("offset", &cameraParam_.at(Character::State::kRunAway).offset.x, 0.1f);
			transform.translate = pos + cameraParam_.at(Character::State::kRunAway).offset;
			ImGui::DragFloat3("eulerAngle", &cameraParam_.at(Character::State::kRunAway).eulerAngle.x, 0.1f);
		}
		break;
		default:
			break;
		}
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/StageCamera/StageCamera.json");
			for (uint32_t i = 0; i < cameraParam_.size(); i++) {
				JSON_OBJECT("cameraParam_" + std::to_string(i));
				cameraParam_.at(i).Save();
				JSON_ROOT();
			}
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}

void StageCamera::SetRenderManager() {
	RenderManager::GetInstance()->SetCamera(camera_);
}

void StageCamera::Reset() {
	transform.translate = Vector3::zero;
	isMove_ = true;
}

void StageCamera::CameraParameter::Load() {
	JSON_LOAD(offset);
	JSON_LOAD(eulerAngle);
	JSON_LOAD(cameraVelocity);
}

void StageCamera::CameraParameter::Save() {
	JSON_SAVE(offset);
	JSON_SAVE(eulerAngle);
	JSON_SAVE(cameraVelocity);
}
