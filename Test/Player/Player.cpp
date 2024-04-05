#include "Player.h"

#include <fstream>

#include "CollisionAttribute.h"
#include "Graphics/ImGuiManager.h"
#include "FollowCamera.h"
#include "File/JsonHelper.h"
#include "Graphics/ResourceManager.h"
#include "Input/Input.h"

void Player::Initialize() {
	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("player"));
	model_->SetIsActive(true);


	transform.translate = Vector3::zero;
	transform.rotate = Quaternion::identity;
	transform.scale = Vector3::one;

	onGround_ = true;
#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Player");
	collider_->SetCenter(transform.translate);
	collider_->SetSize({ 1.0f, 2.0f, 1.0f });
	//collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Player);
	collider_->SetCollisionMask(~CollisionAttribute::Player);
	//collider_->SetIsActive(false);
#pragma endregion

	//playerModel_.Initialize(&transform);
	//playerModel_.PlayAnimation(PlayerModel::kWait, true);

#pragma region パラメーター
	JSON_OPEN("Resources/Data/Player/Player.json");
	JSON_OBJECT("Player");
	JSON_LOAD(defaultSpeed_);
	JSON_LOAD(verticalSpeed_);
	JSON_LOAD(horizontalSpeed_);
	JSON_LOAD(jumpPower_);
	JSON_LOAD(gravity_);
	JSON_CLOSE();
#pragma endregion

}

void Player::Update() {
	Move();

	Jump();

	DebugParam();

	// 地面についているとき
	if (!onGround_) {
		acceleration_.y += gravity_;
	}
	if (transform.translate.y < 0.0f) {
		transform.translate.y = 0.0f;
		acceleration_.y = 0.0f;
		velocity_.y = 0.0f;
		onGround_ = true;
	}
	velocity_ += acceleration_;
	transform.translate += velocity_;
	transform.translate.x = std::clamp(transform.translate.x, -20.0f, 20.0f);
	transform.UpdateMatrix();
	//playerModel_.Update();
	model_->SetWorldMatrix(transform.worldMatrix);
}

//void Player::OnCollision(const CollisionInfo& collisionInfo) {
//	collisionInfo;
//}

void Player::Move() {
	auto input = Input::GetInstance();

	Vector3 move{};
	// Gamepad入力
	auto& xinputState = input->GetXInputState();

	const float margin = 0.8f;
	const float shortMaxReci = 1.0f / float(SHRT_MAX);
	move = { float(xinputState.Gamepad.sThumbLX), 0.0f, float(xinputState.Gamepad.sThumbLY) };
	move *= shortMaxReci;
	if (move.Length() < margin) {
		move = Vector3::zero;
	}
	if (input->IsKeyPressed(DIK_A)) {
		move.x = -1.0f;
	}
	if (input->IsKeyPressed(DIK_D)) {
		move.x = 1.0f;
	}
	if (input->IsKeyPressed(DIK_S)) {
		move.z = -1.0f;
	}
	if (input->IsKeyPressed(DIK_W)) {
		move.z = 1.0f;
	}

	if (move != Vector3::zero) {
		move = move.Normalized();
		// 地面に水平なカメラの回転
		/*if (followCamera) {
			move = followCamera->GetCamera()->GetRotate() * move;
		}*/
		move.y = 0.0f;
		move = move.Normalized();

		move.x *= horizontalSpeed_;
		move.z *= verticalSpeed_;
		move.z += defaultSpeed_;

		// 親がいる場合親の空間にする
		const Transform* parent = transform.GetParent();
		if (parent) {
			move = parent->worldMatrix.Inverse().ApplyRotation(move);
		}
		// 回転
		transform.rotate = Quaternion::Slerp(0.2f, transform.rotate, Quaternion::MakeLookRotation({ move.x,0.0f,move.z }));

		Vector3 vector = transform.rotate.Conjugate() * move;
		Quaternion diff = Quaternion::MakeFromTwoVector(Vector3::unitZ, vector);
		transform.rotate = Quaternion::Slerp(0.2f, Quaternion::identity, diff) * transform.rotate;
		/*if (playerModel_.GetAnimationType() != PlayerModel::AnimationType::kWalk) {
			playerModel_.PlayAnimation(PlayerModel::kWalk, true);
		}*/
	}
	else {
		move.z += defaultSpeed_;

		/*if (playerModel_.GetAnimationType() != PlayerModel::AnimationType::kWait) {
			playerModel_.PlayAnimation(PlayerModel::kWait, true);
		}*/
	}
	// 移動処理
	velocity_ = Vector3::zero;
	// 移動
	velocity_ += move;
}

void Player::Jump() {
	// ジャンプ
	if (onGround_ &&
		(Input::GetInstance()->IsKeyTrigger(DIK_SPACE) || (Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A))) {
		acceleration_.y = jumpPower_;
		onGround_ = false;
	}
}

void Player::DebugParam() {
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::DragFloat3("Pos", &transform.translate.x);
	ImGui::DragFloat3("velocity_", &velocity_.x);
	ImGui::DragFloat("defaultSpeed_", &defaultSpeed_);
	ImGui::DragFloat("verticalSpeed_", &verticalSpeed_);
	ImGui::DragFloat("horizontalSpeed_", &horizontalSpeed_);
	ImGui::DragFloat("jumpPower_", &jumpPower_);
	ImGui::DragFloat("gravity_", &gravity_);
	if (ImGui::Button("Save")) {
		JSON_OPEN("Resources/Data/Player/Player.json");
		JSON_OBJECT("Player");
		JSON_SAVE(defaultSpeed_);
		JSON_SAVE(verticalSpeed_);
		JSON_SAVE(horizontalSpeed_);
		JSON_SAVE(jumpPower_);
		JSON_SAVE(gravity_);
		JSON_CLOSE();
	}
	ImGui::End();
#endif // _DEBUG
}