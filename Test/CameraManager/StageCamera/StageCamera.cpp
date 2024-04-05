#include "StageCamera.h"

#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"
#include "File/JsonHelper.h"
#include "Math/MathUtils.h"
#include "Input/Input.h"
#include "Player/Player.h"

void StageCamera::Initialize() {
	camera_ = std::make_shared<Camera>();
	RenderManager::GetInstance()->SetCamera(camera_);

	transform.rotate = Quaternion::MakeForXAxis(10.0f * Math::ToRadian);

#pragma region パラメーター
	JSON_OPEN("Resources/Data/StageCamera/StageCamera.json");
	JSON_OBJECT("StageCamera");
	JSON_LOAD(offset_);
	JSON_LOAD(eulerAngle_);
	JSON_LOAD(cameraVelocity_);
	JSON_CLOSE();
#pragma endregion
	//offset_ = { 0.0f,25.0f,-70.0f };
	//eulerAngle_ = Vector3(30.0f, 0.0f, 0.0f);
	//cameraVelocity_ = { 0.0f,0.0f,-0.5f };
}

void StageCamera::Update() {
	transform.translate += cameraVelocity_;
	camera_->SetPosition({ transform.translate.x + offset_.x,transform.translate.y + offset_.y,transform.translate.z + offset_.z });
	camera_->SetRotate(Quaternion::MakeFromEulerAngle(eulerAngle_ * Math::ToRadian));
	camera_->UpdateMatrices();
#ifdef _DEBUG
	ImGui::Begin("StageCamera");
	ImGui::DragFloat3("Pos",&transform.translate.x,0.1f);
	ImGui::DragFloat3("Offset",&offset_.x,0.1f);
	ImGui::DragFloat3("eulerAngle_",&eulerAngle_.x,0.1f);
	ImGui::DragFloat3("cameraVelocity_",&cameraVelocity_.x,0.1f);
	if(ImGui::Button("Save")) {
		JSON_OPEN("Resources/Data/StageCamera/StageCamera.json");
		JSON_OBJECT("StageCamera");
		JSON_SAVE(offset_);
		JSON_SAVE(eulerAngle_);
		JSON_SAVE(cameraVelocity_);
		JSON_CLOSE();
	}
	ImGui::End();
#endif // _DEBUG
}

void StageCamera::SetRenderManager() {
	RenderManager::GetInstance()->SetCamera(camera_);
}

void StageCamera::Reset() {
	transform.translate = Vector3::zero;
}
