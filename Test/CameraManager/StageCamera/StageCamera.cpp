#include "StageCamera.h"

#include "Externals/ImGui/imgui.h"
#include "Graphics/RenderManager.h"
#include "Math/MathUtils.h"
#include "Input/Input.h"
#include "Player/Player.h"

void StageCamera::Initialize() {
	camera_ = std::make_shared<Camera>();
	RenderManager::GetInstance()->SetCamera(camera_);

	transform.rotate = Quaternion::MakeForXAxis(10.0f * Math::ToRadian);

	offset_ = { 0.0f,25.0f,-70.0f };
	eulerAngle_ = Vector3(30.0f, 0.0f, 0.0f);
	cameraVelocity_ = { 0.0f,0.0f,-1.0f };
}

void StageCamera::Update() {
#ifdef _DEBUG
	ImGui::Begin("StageCamera");
	ImGui::DragFloat3("Offset",&offset_.x,0.1f);
	ImGui::DragFloat3("eulerAngle_",&eulerAngle_.x,0.1f);
	ImGui::End();
#endif // _DEBUG
	transform.translate += cameraVelocity_;
	camera_->SetPosition({ transform.translate.x + offset_.x,transform.translate.y + offset_.y,transform.translate.z + offset_.z });
	camera_->SetRotate(Quaternion::MakeFromEulerAngle(eulerAngle_ * Math::ToRadian));
	camera_->UpdateMatrices();
}

void StageCamera::SetRenderManager() {
	RenderManager::GetInstance()->SetCamera(camera_);
}
