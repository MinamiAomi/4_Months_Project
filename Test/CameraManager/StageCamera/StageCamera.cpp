#include "StageCamera.h"

#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"
#include "File/JsonHelper.h"
#include "Math/MathUtils.h"
#include "Input/Input.h"
#include "Player/Player.h"
#include "Boss/Boss.h"
#include "GameSpeed.h"

void StageCamera::Initialize() {
	camera_ = std::make_shared<Camera>();
	RenderManager::GetInstance()->SetCamera(camera_);


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
	Reset();
}

void StageCamera::Update() {
	
	switch (Character::currentCharacterState_) {
	case Character::State::kChase:
	{
		if (isMove_) {
			transform.translate.z += GameSpeed::GetGameSpeed();
			transform.UpdateMatrix();
		}
		float t = std::clamp((boss_->transform.translate.z - player_->transform.translate.z) / player_->GetChaseLimitLine(),0.0f,1.0f);
		float offsetZ = std::lerp(cameraParam_.at(Character::State::kChase).offset.z * -1.0f, cameraParam_.at(Character::State::kChase).offset.z, t);
		camera_->SetPosition(
			{
			transform.translate.x + cameraParam_.at(Character::State::kChase).offset.x,
			transform.translate.y + cameraParam_.at(Character::State::kChase).offset.y,
			transform.translate.z + offsetZ
			});
		camera_->SetRotate(Quaternion::MakeFromEulerAngle(cameraParam_.at(Character::State::kChase).eulerAngle * Math::ToRadian));
	}
	break;
	case Character::State::kRunAway:
	{
		if (isMove_) {
			transform.translate.z -= GameSpeed::GetGameSpeed();
			transform.UpdateMatrix();
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
	case Character::State::kScneChange:
	{


		float t = Character::GetSceneChangeTime();
		Vector3 offset{}, rotate{};
		switch (Character::nextCharacterState_) {
		case Character::State::kChase:
		{

			if (Character::IsInSceneChange()) {
				easingStartPosition_ = transform.translate;
			}

			transform.translate.z = std::lerp(easingStartPosition_.z,player_->transform.worldMatrix.GetTranslate().z, t);
			offset = {
			std::lerp(cameraParam_.at(Character::State::kRunAway).offset.x, cameraParam_.at(Character::State::kChase).offset.x, t),
			std::lerp(cameraParam_.at(Character::State::kRunAway).offset.y, cameraParam_.at(Character::State::kChase).offset.y, t),
			std::lerp(cameraParam_.at(Character::State::kRunAway).offset.z, cameraParam_.at(Character::State::kChase).offset.z, t),
			};
			rotate = {
			std::lerp(cameraParam_.at(Character::State::kRunAway).eulerAngle.x, cameraParam_.at(Character::State::kChase).eulerAngle.x, t),
			std::lerp(cameraParam_.at(Character::State::kRunAway).eulerAngle.y, cameraParam_.at(Character::State::kChase).eulerAngle.y, t),
			std::lerp(cameraParam_.at(Character::State::kRunAway).eulerAngle.z, cameraParam_.at(Character::State::kChase).eulerAngle.z, t),
			};

		}
		break;
		case Character::State::kRunAway:
		{

			if (Character::IsInSceneChange()) {
				easingStartPosition_ = transform.translate;
				easingStartOffset_ = camera_->GetPosition() - transform.translate;
			}

			transform.translate.z = std::lerp(easingStartPosition_.z, boss_->transform.worldMatrix.GetTranslate().z, t);
			offset = {
			std::lerp(easingStartOffset_.x, cameraParam_.at(Character::State::kRunAway).offset.x, t),
			std::lerp(easingStartOffset_.y, cameraParam_.at(Character::State::kRunAway).offset.y, t),
			std::lerp(easingStartOffset_.z, cameraParam_.at(Character::State::kRunAway).offset.z, t),
			};
			rotate = {
			std::lerp(cameraParam_.at(Character::State::kChase).eulerAngle.x, cameraParam_.at(Character::State::kRunAway).eulerAngle.x, t),
			std::lerp(cameraParam_.at(Character::State::kChase).eulerAngle.y, cameraParam_.at(Character::State::kRunAway).eulerAngle.y, t),
			std::lerp(cameraParam_.at(Character::State::kChase).eulerAngle.z, cameraParam_.at(Character::State::kRunAway).eulerAngle.z, t),
			};
		}
		break;
		default:
			break;
		}
		transform.UpdateMatrix();
		camera_->SetPosition(
			{
			transform.translate.x + offset.x,
			transform.translate.y + offset.y,
			transform.translate.z + offset.z
			});
		camera_->SetRotate(Quaternion::MakeFromEulerAngle(rotate * Math::ToRadian));
		break;
	}

	default:
		break;
	}
	camera_->UpdateMatrices();
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("CameraManager")) {

		ImGui::Text("Pos: x:%f,y:%f,z:%f", camera_->GetPosition().x, camera_->GetPosition().y, camera_->GetPosition().z, 0.1f);
		ImGui::Text("Translate: x:%f,y:%f,z:%f", transform.translate.x, transform.translate.y, transform.translate.z, 0.1f);
		switch (Character::currentCharacterState_) {
		case Character::State::kChase:
		{
			ImGui::DragFloat3("cameraVelocity", &cameraParam_.at(Character::State::kChase).cameraVelocity.x, 0.1f);
			ImGui::DragFloat3("offset", &cameraParam_.at(Character::State::kChase).offset.x, 0.1f);
			ImGui::DragFloat3("eulerAngle", &cameraParam_.at(Character::State::kChase).eulerAngle.x, 0.1f);

		}
		break;
		case Character::State::kRunAway:
		{
			ImGui::DragFloat3("cameraVelocity", &cameraParam_.at(Character::State::kRunAway).cameraVelocity.x, 0.1f);
			ImGui::DragFloat3("offset", &cameraParam_.at(Character::State::kRunAway).offset.x, 0.1f);
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
	transform.translate = Vector3(0.0f,0.0f, player_->transform.worldMatrix.GetTranslate().z);
	camera_->SetPosition(transform.translate + cameraParam_.at(Character::State::kChase).offset);
	camera_->SetRotate(Quaternion::MakeFromEulerAngle(cameraParam_.at(Character::State::kChase).eulerAngle * Math::ToRadian));
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
