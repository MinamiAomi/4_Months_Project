#include "DebugCamera.h"

#include "Graphics/RenderManager.h"
#include "Graphics/ImGuiManager.h"
#include "Input/Input.h"

void DebugCamera::Initialize() {
	camera_ = std::make_shared<Camera>();
	RenderManager::GetInstance()->SetCamera(camera_);

	transform.rotate = Quaternion::MakeForXAxis(10.0f * Math::ToRadian);

	eulerAngle_ = Vector3(10.0f, 0.0f, 0.0f) * Math::ToRadian;

}

void DebugCamera::Update() {
	Input* input = Input::GetInstance();

	auto mouseMoveX = input->GetMouseMoveX();
	auto mouseMoveY = input->GetMouseMoveY();
	auto wheel = input->GetMouseWheel();

	Quaternion rotate = camera_->GetRotate();
	Vector3 position = camera_->GetPosition();

	Vector3 diffPosition;

	if (input->IsMousePressed(1)) {
		constexpr float rotSpeed = Math::ToRadian * 0.1f;
		eulerAngle_.x += rotSpeed * static_cast<float>(mouseMoveY);
		eulerAngle_.y += rotSpeed * static_cast<float>(mouseMoveX);
	}
	else if (input->IsMousePressed(2)) {
		Vector3 cameraX = rotate.GetRight() * (-static_cast<float>(mouseMoveX) * 0.02f);
		Vector3 cameraY = rotate.GetUp() * (static_cast<float>(mouseMoveY) * 0.02f);
		diffPosition += cameraX + cameraY;
	}
	else if (wheel != 0) {
		Vector3 cameraZ = rotate.GetForward() * (static_cast<float>(wheel / 120) * 2.5f);
		diffPosition += cameraZ;
	}


	camera_->SetPosition(position + diffPosition);
	camera_->SetRotate(Quaternion::MakeFromEulerAngle(eulerAngle_));
	camera_->UpdateMatrices();

#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("CameraManager")) {
		ImGui::Text("Pos:%f,%f,%f", (position.x + diffPosition.x), (position.y + diffPosition.y), (position.z + diffPosition.z));
		ImGui::Text("Rotate:%f,%f,%f", eulerAngle_.x, eulerAngle_.y, eulerAngle_.z);
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG

}

void DebugCamera::SetRenderManager() {
	RenderManager::GetInstance()->SetCamera(camera_);
}
