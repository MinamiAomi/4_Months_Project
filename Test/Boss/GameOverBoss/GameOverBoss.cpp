#include "GameOverBoss.h"

#include "Framework/ResourceManager.h"
#include "Engine/Graphics/ImGuiManager.h"

void GameOverBoss::Initialize() {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("boss_2"));
	model_->SetIsActive(true);

	animation_ = ResourceManager::GetInstance()->FindAnimation("boss_2");
	skeleton_ = std::make_shared<Skeleton>();
	skeleton_->Create(model_->GetModel());
	animationAllFrame_ = 240.0f;
	animationTime_ = 0.0f;
	offset_ = 50.0f;
	skeleton_->ApplyAnimation(animation_->GetAnimation("GameOver"), 0.0f);
	model_->SetSkeleton(skeleton_);
}


void GameOverBoss::Update() {
	transform.translate.z = offset_;
	animationTime_ += 1.0f;
	animationTime_ = std::fmodf(animationTime_, animationAllFrame_);
	skeleton_->ApplyAnimation(animation_->GetAnimation("GameOver"), animationTime_ / animationAllFrame_);
	skeleton_->Update();
	UpdateTransform();
#ifdef _DEBUG
	if (ImGui::BeginMenu("GameOverBoss")) {
		ImGui::DragFloat("アニメーション全体フレーム", &animationAllFrame_, 0.1f, 0.0f);
		ImGui::DragFloat("offset", &offset_, 0.1f, 0.0f);
		ImGui::EndMenu();
	}
#endif // _DEBUG
}

void GameOverBoss::UpdateTransform() {
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}