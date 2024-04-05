#include "Boss.h"

#include "Graphics/ResourceManager.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"

void Boss::Initialize() {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("boss"));
	model_->SetIsActive(true);

	transform.translate = { 0.0f,20.0f,50.0f };
	transform.rotate = Quaternion::identity;
	transform.scale = { 10.0f,10.0f,10.0f };

	isMove_ = true;

#pragma region パラメーター
	JSON_OPEN("Resources/Data/Boss/Boss.json");
	JSON_OBJECT("Boss");
	JSON_LOAD(velocity_);
	JSON_CLOSE();
#pragma endregion
}

void Boss::Update() {
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Boss")) {
		ImGui::DragFloat3("velocity_", &velocity_.x, 0.1f);
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/Boss/Boss.json");
			JSON_OBJECT("Boss");
			JSON_SAVE(velocity_);
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
	if (isMove_) {
		transform.translate += velocity_;
	}
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Boss::Reset() {
	transform.translate = { 0.0f,20.0f,50.0f };
	transform.rotate = Quaternion::identity;
	transform.scale = { 10.0f,10.0f,10.0f };
}
