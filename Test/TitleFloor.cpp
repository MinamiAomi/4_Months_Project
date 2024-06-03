#include "TitleFloor.h"

#include "Framework/ResourceManager.h"
#include "Engine/Graphics/ImGuiManager.h"
#include "Math/MathUtils.h"

void TitleFloor::Initialize() {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("floor"));
	model_->SetIsActive(true);
	transform.translate.y = -1.5f;
	transform.rotate = Quaternion::MakeForYAxis(90.0f * Math::ToRadian);
}

void TitleFloor::Update() {
#ifdef _DEBUG
	if (ImGui::BeginMenu("TitleFloor")) {
		ImGui::DragFloat3("translate", &transform.translate.x, 0.1f);
		ImGui::EndMenu();
	}
#endif // _DEBUG

	UpdateTransform();
}

void TitleFloor::UpdateTransform() {
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}
