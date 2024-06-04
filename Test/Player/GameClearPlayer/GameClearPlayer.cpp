#include "GameClearPlayer.h"

#include "Framework/ResourceManager.h"
#include "Engine/Graphics/ImGuiManager.h"
#include "Math/MathUtils.h"

void GameClearPlayer::Initialize() {
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

	transform.translate = { -1.0f,11.0f,0.0f };
	//transform.rotate = Quaternion::MakeForYAxis(180.0f * Math::ToRadian);
	UpdateTransform();
	ground_ = 0.0f;
	maxJumpTime_ = 120.0f;
	ground_ = 11.0f;
	gravity_ = 0.01f;
	jumpPower_ = 0.3f;
	maxRotateTime_ = 60.0f;
	isFirst_ = true;
}

void GameClearPlayer::Update() {
	Jump();
	acceleration_ -= gravity_;
	float velocity = acceleration_;
	transform.translate.y += velocity;
	transform.translate.y = std::clamp(transform.translate.y, ground_, FLT_MAX);
	if (transform.translate.y <= ground_) {
		acceleration_ = 0.0f;
	}
	animationTime_ += 1.0f;
	animationTime_ = std::fmodf(animationTime_, animationAllFrame_);
	skeleton_->ApplyAnimation(animation_->GetAnimation("move.001"), animationTime_ / animationAllFrame_);
	skeleton_->Update();
	UpdateTransform();
#ifdef _DEBUG
	if (ImGui::BeginMenu("TitlePlayer")) {
		ImGui::DragFloat("アニメーション全体フレーム", &animationAllFrame_, 0.1f, 0.0f);
		ImGui::DragFloat3("Position", &transform.translate.x, 0.1f);
		ImGui::DragFloat("Gravity", &gravity_, 0.01f);
		ImGui::DragFloat("Ground", &ground_, 0.1f);
		ImGui::DragFloat("JumpPower", &jumpPower_, 0.1f);
		ImGui::DragFloat("MaxJumpTime", &maxJumpTime_, 0.1f);
		ImGui::DragFloat("MaxRotateTime", &maxRotateTime_, 0.1f);
		ImGui::EndMenu();
	}
#endif // _DEBUG
}

void GameClearPlayer::Jump() {
	if (jumpTime_ >= maxJumpTime_) {
		acceleration_ = jumpPower_;
		jumpTime_ = 0.0f;
		isFirst_ = false;
	}
	else {
		jumpTime_ += 1.0f;
		if (!isFirst_) {
			float t = jumpTime_ / maxRotateTime_;
			t = std::clamp(t, 0.0f, 1.0f);
			transform.rotate = Quaternion::MakeForXAxis(Math::TwoPi * t);
		}
	}
}

void GameClearPlayer::UpdateTransform() {
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
}
