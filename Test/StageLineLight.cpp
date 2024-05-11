#include "StageLineLight.h"

#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "Graphics/ImGuiManager.h"
#include "Graphics/RenderManager.h"

void StageLineLight::Initialize(bool isLeft,bool isUp) {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("lightline"));
	model_->SetIsActive(false);
	lightManager_ = &RenderManager::GetInstance()->GetLightManager();

	originTransform_.SetParent(&transform);
	diffTransform_.SetParent(&transform);

	originTransform_.translate.z = 300.0f;
	diffTransform_.translate.z = -300.0f;

	lineLight_ = std::make_unique<LineLight>();

	if (isLeft) {
		transform.translate.x = -32.0f;
	}
	else {
		transform.translate.x = 32.0f;
	}

	if (isUp) {
		transform.translate.y = 6.4f;
		lineLight_->range = 10.5f;
	}
	else {
		transform.translate.y = -10.0f;
		lineLight_->range = 15.0f;
	}

	lineLight_->color = runAwayColor_;
	lineLight_->intensity = 1.5f;
	lineLight_->decay = 0.85f;
	

	saveState_ = Character::currentCharacterState_;
}

void StageLineLight::Update() {
	UpdateTransform();
#ifdef _DEBUG
	ImGui::Begin("StageLineLight");
	ImGui::DragFloat3("Color", &lineLight_->color.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("intensity", &lineLight_->intensity, 0.01f, 0.1f);
	ImGui::DragFloat("decay", &lineLight_->decay, 0.01f, 0.0f);
	ImGui::DragFloat("range", &lineLight_->range, 0.01f, 0.0f);
	ImGui::DragFloat3("StageLineLightTranslate", &transform.translate.x, 0.1f, 0.0f);
	ImGui::DragFloat3("StageLineLightScale", &transform.scale.x, 0.1f, 0.0f);
	ImGui::DragFloat3("StageLineLightRotate", &transform.rotate.x, 0.1f, 0.0f);
	ImGui::DragFloat3("LineLightTranslate", &originTransform_.translate.x, 0.1f, 0.0f);
	ImGui::End();
#endif // _DEBUG

	//�J��
	if (saveState_ != Character::currentCharacterState_) {
		if (Character::currentCharacterState_ == Character::kChase) {
			//�����ɂȂ�����
			t_ += speed_;
			t_ = std::clamp(t_, 0.0f, 1.0f);
			if (t_ >= 1.0f) {
				saveState_ = Character::currentCharacterState_;
			}
		}
		else {
			//�����鑤�ɂȂ�����
			t_ -= speed_;
			t_ = std::clamp(t_, 0.0f, 1.0f);
			if (t_ <= 0.0f) {
				saveState_ = Character::currentCharacterState_;
			}
		}
	}

	lineLight_->color = Vector3::Lerp(t_, runAwayColor_, ChaseColor_);

	lightManager_->Add(lineLight_);
}

void StageLineLight::UpdateTransform() {
	if (player_) {
		transform.translate.z = player_->GetWorldMatrix().GetTranslate().z;
	}
	transform.UpdateMatrix();
	originTransform_.UpdateMatrix();
	diffTransform_.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
	lineLight_->origin = originTransform_.worldMatrix.GetTranslate();
	lineLight_->diff = diffTransform_.worldMatrix.GetTranslate();
	lineLight_->diff = lineLight_->diff - lineLight_->origin;
}

