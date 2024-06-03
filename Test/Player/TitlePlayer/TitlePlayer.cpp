#include "TitlePlayer.h"

#include "Framework/ResourceManager.h"
#include "Engine/Graphics/ImGuiManager.h"
#include "Math/MathUtils.h"

void TitlePlayer::Initialize() {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("player"));
	model_->SetIsActive(true);

	animation_ = ResourceManager::GetInstance()->FindAnimation("player");
	skeleton_ = std::make_shared<Skeleton>();
	skeleton_->Create(model_->GetModel());
	animationAllFrame_ = 60.0f;
	animationTime_ = 0.0f;
	skeleton_->ApplyAnimation(animation_->GetAnimation("move.001"), 0.0f);
	model_->SetSkeleton(skeleton_);

	transform.rotate = Quaternion::MakeForYAxis(180.0f*Math::ToRadian);
}

void TitlePlayer::Update() {
	animationTime_ += 1.0f;
	animationTime_ = std::fmodf(animationTime_, animationAllFrame_);
	skeleton_->ApplyAnimation(animation_->GetAnimation("move.001"), animationTime_ / animationAllFrame_);
	skeleton_->Update();
	UpdateTransform();
#ifdef _DEBUG
	if (ImGui::BeginMenu("TitlePlayer")) {
		ImGui::DragFloat("アニメーション全体フレーム", &animationAllFrame_, 0.1f, 0.0f);
		ImGui::EndMenu();
	}
#endif // _DEBUG
}

void TitlePlayer::UpdateTransform() {
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}
