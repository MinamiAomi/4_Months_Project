#include "Player.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ImGuiManager.h"
#include "File/JsonHelper.h"
#include "FollowCamera.h"
#include "Framework/ResourceManager.h"
#include "Input/Input.h"


void Player::Initialize() {
#pragma region パラメーター
	JSON_OPEN("Resources/Data/Player/Player.json");
	JSON_OBJECT("Player");
	JSON_LOAD(verticalSpeed_);
	JSON_LOAD(horizontalSpeed_);
	JSON_LOAD(jumpPower_);
	JSON_LOAD(gravity_);
	JSON_LOAD(chaseLimitLine_);
	JSON_LOAD(runAwayLimitLine_);
	JSON_LOAD(knockBack_);
	JSON_LOAD(maxInvincibleTime_);
	JSON_LOAD(offset_);
	JSON_LOAD(revengeStartOffset_);
	JSON_CLOSE();
#pragma endregion

	model_ = std::make_unique<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("player"));
	model_->SetIsActive(true);

	playerHP_ = std::make_unique<PlayerHP>();
	playerUI_ = std::make_unique<PlayerUI>();
	playerRevengeGage_ = std::make_unique<PlayerRevengeGage>();

	bulletManager_ = std::make_unique<BulletManager>();

	playerHP_->Initialize();
	playerUI_->SetPlayerHP(playerHP_.get());
	playerUI_->SetPlaterRevengeGage(playerRevengeGage_.get());
	playerUI_->Initialize();
	playerRevengeGage_->Initialize();

	bulletManager_->Initialize();

	Reset();
#pragma region SE
	jumpSE_ = std::make_unique<AudioSource>();
	revengeSE_ = std::make_unique<AudioSource>();

	(*jumpSE_) = ResourceManager::GetInstance()->FindSound("jump");
	(*revengeSE_) = ResourceManager::GetInstance()->FindSound("revenge");
#pragma endregion

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Player");
	collider_->SetCenter(transform.translate);
	collider_->SetSize({ 1.0f, 2.0f, 1.0f });
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Player);
	collider_->SetCollisionMask(~CollisionAttribute::Player);
	collider_->SetIsActive(true);
#pragma endregion

	//playerModel_.Initialize(&transform);
	//playerModel_.PlayAnimation(PlayerModel::kWait, true);
}

void Player::Update() {

	switch (Character::currentCharacterState_) {
	case Character::State::kChase:
	case Character::State::kRunAway:
		//false
		isGround_ = false;
		isMove_ = false;

		if (playerHP_->GetCurrentHP() <= 0) {
			isAlive_ = false;
		}

		// 移動
		Move();

		// ジャンプ
		Jump();

		// 無敵
		Invincible();

		// リベンジゲージ
		playerRevengeGage_->Update();

		// HP
		playerHP_->Update();

		// UIアップデート
		playerUI_->Update();

		// 切り替え
		if (Character::currentCharacterState_ == Character::State::kRunAway &&
			(playerRevengeGage_->GetCurrentRevengeBarGage() >= PlayerRevengeGage::kMaxRevengeBar) &&
			(Input::GetInstance()->IsKeyTrigger(DIK_J) || (Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_B))) {
			Character::SetNextScene(Character::State::kChase);
			//transform.translate.x = 0.0f;
			//transform.translate.z = boss_->transform.worldMatrix.GetTranslate().z - chaseLimitLine_;
			transform.rotate = Quaternion::identity;
			canFirstJump_ = true;
			canSecondJump_ = true;
			velocity_ = Vector3::zero;
			acceleration_ = Vector3::zero;
			revengeSE_->Play();
		}
		if (Character::currentCharacterState_ == Character::State::kChase &&
			playerRevengeGage_->GetCurrentRevengeBarGage() <= 0) {
			Character::SetNextScene(Character::State::kRunAway);
		}
		acceleration_.y += gravity_;
		acceleration_.z *= 0.9f;
		velocity_ += acceleration_;
		transform.translate += velocity_;

		transform.translate.x = std::clamp(transform.translate.x, -20.0f, 20.0f);
		transform.translate.y = std::max(transform.translate.y, -10.0f);
		// 救済
		if (transform.translate.y <= -10.0f) {
			transform.translate.y = 8.0f;
			acceleration_.y = 0.0f;
			// ジャンプ復活
			canFirstJump_ = true;
			canSecondJump_ = true;
		}
		switch (Character::currentCharacterState_) {
		case Character::kChase:
		{
			transform.translate.z = std::clamp(transform.translate.z, boss_->transform.translate.z - chaseLimitLine_, boss_->transform.translate.z);
		}
		break;
		case Character::kRunAway:
		{
			transform.translate.z = std::clamp(transform.translate.z, boss_->transform.translate.z - runAwayLimitLine_, boss_->transform.translate.z);
		}
		break;
		case Character::kCount:
			break;
		default:
			break;
		}
		break;
	case Character::State::kScneChange:

		break;
	default:
		break;
	}
	DebugParam();
	UpdateTransform();


	// 弾アップデート
	//bulletManager_->Update(transform.worldMatrix.GetTranslate());

	//playerModel_.Update();
}

void Player::Reset() {
	invincibleTime_ = 0;
	transform.translate = offset_;
	transform.rotate = Quaternion::identity;
	transform.scale = Vector3::one;
	transform.UpdateMatrix();
	canFirstJump_ = true;
	canSecondJump_ = true;
	isAlive_ = true;
	velocity_ = Vector3::zero;
	acceleration_ = Vector3::zero;
	playerHP_->Reset();
	playerRevengeGage_->Reset();
}

void Player::UpdateTransform() {
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Player::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Boss") {
		switch (Character::currentCharacterState_) {
		case Character::State::kChase:
		{

		}
		break;
		case Character::State::kRunAway:
		{
			acceleration_.z -= knockBack_;
			if (invincibleTime_ == 0) {
				invincibleTime_ = maxInvincibleTime_;
				if (playerHP_->GetCurrentHP() > 0) {
					playerHP_->AddHP(-1);
				}
			}
		}
		break;
		default:
			break;
		}
	}
	else if (collisionInfo.collider->GetName() == "Block" ||
		collisionInfo.collider->GetName() == "FireBarCenter" ||
		collisionInfo.collider->GetName() == "Floor" ||
		collisionInfo.collider->GetName() == "StageObject") {
		// ワールド空間の押し出しベクトル
		Vector3 pushVector = collisionInfo.normal * collisionInfo.depth;
		auto parent = transform.GetParent();
		if (parent) {
			pushVector = parent->rotate.Inverse() * pushVector;
		}
		transform.translate += pushVector;
		// 上から乗ったら
		if (std::fabs(Dot(collisionInfo.normal, Vector3::down)) >= 0.5f) {
			//transform.translate.y = collisionInfo.collider->GetGameObject()->transform.translate.y + collisionInfo.collider->GetGameObject()->transform.scale.y * 0.5f;
			if (acceleration_.y < 0.0f) {
				acceleration_.y = 0.0f;
			}
			//velocity_.y = 0.0f;
			canFirstJump_ = true;
			canSecondJump_ = true;
			isGround_ = true;
		}

		UpdateTransform();
		//const GameObject* nextParent = collisionInfo.collider->GetGameObject();
		//if (nextParent) {
		//	transform.SetParent(&nextParent->transform);
		//}
	}
	else if (collisionInfo.collider->GetName() == "FireBarBar" ||
		collisionInfo.collider->GetName() == "PendulumBall" ||
		collisionInfo.collider->GetName() == "bossLeftArm" ||
		collisionInfo.collider->GetName() == "bossFloorAll"||
		collisionInfo.collider->GetName() == "bossLongDistanceAttack") {
		if (invincibleTime_ == 0) {
			invincibleTime_ = maxInvincibleTime_;
			if (playerHP_->GetCurrentHP() > 0) {
				playerHP_->AddHP(-1);
			}
		}
	}
	else if (collisionInfo.collider->GetName() == "RevengeCoin") {
		playerRevengeGage_->AddGage();
	}

}

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
		isMove_ = true;
		move = move.Normalized();
		// 地面に水平なカメラの回転
		/*if (followCamera) {
			move = followCamera->GetCamera()->GetRotate() * move;
		}*/
		move.y = 0.0f;
		move = move.Normalized();

		move.x *= horizontalSpeed_;
		move.z *= verticalSpeed_;

		// 親がいる場合親の空間にする
		const Transform* parent = transform.GetParent();
		if (parent) {
			move = parent->worldMatrix.Inverse().ApplyRotation(move);
		}
		// 回転
		transform.rotate = Quaternion::Slerp(0.1f, transform.rotate, Quaternion::MakeLookRotation({ move.x,0.0f,move.z }));

		Vector3 vector = transform.rotate.Conjugate() * move.Normalized();
		if (Vector3::Dot(Vector3::unitZ, vector) < 0.999f) {
			Quaternion diff = Quaternion::MakeFromTwoVector(Vector3::unitZ, vector);
			transform.rotate = Quaternion::Slerp(0.2f, Quaternion::identity, diff) * transform.rotate;
		}
		/*if (playerModel_.GetAnimationType() != PlayerModel::AnimationType::kWalk) {
			playerModel_.PlayAnimation(PlayerModel::kWalk, true);
		}*/
	}
	else {
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
	if (canFirstJump_ &&
		(Input::GetInstance()->IsKeyTrigger(DIK_SPACE) ||
			((Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
				!(Input::GetInstance()->GetPreXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A)))) {
		jumpSE_->Play();
		acceleration_.y += jumpPower_;
		canFirstJump_ = false;
	}
	else if (!canFirstJump_ &&
		canSecondJump_ &&
		(Input::GetInstance()->IsKeyTrigger(DIK_SPACE) ||
			((Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
				!(Input::GetInstance()->GetPreXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A)))) {
		jumpSE_->Play();
		canSecondJump_ = false;
		acceleration_.y = jumpPower_ * 0.5f;
	}
}

void Player::Invincible() {
	if (invincibleTime_ > 0) {
		model_->SetColor({ 1.0f,0.0f,0.0f });
		invincibleTime_--;
	}
	else {
		model_->SetColor({ 1.0f,1.0f,1.0f });
	}
}

void Player::DebugParam() {
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Player")) {
		ImGui::DragFloat3("Pos", &transform.translate.x);
		ImGui::DragFloat4("rotate", &transform.rotate.x);
		ImGui::DragFloat3("offset_", &offset_.x);
		ImGui::DragFloat3("revengeStartOffset_", &revengeStartOffset_.x);
		ImGui::DragFloat3("velocity_", &velocity_.x);
		ImGui::DragFloat3("acceleration_", &acceleration_.x);
		ImGui::DragFloat("verticalSpeed_", &verticalSpeed_);
		ImGui::DragFloat("horizontalSpeed_", &horizontalSpeed_);
		ImGui::DragFloat("jumpPower_", &jumpPower_);
		ImGui::DragFloat("gravity_", &gravity_);
		ImGui::DragFloat("chaseLimitLine_", &chaseLimitLine_);
		ImGui::DragFloat("runAwayLimitLine_", &runAwayLimitLine_);
		ImGui::DragFloat("knockBack_", &knockBack_);
		int maxInvincibleTime = maxInvincibleTime_;
		ImGui::DragInt("maxInvincibleTime_", &maxInvincibleTime);
		maxInvincibleTime_ = static_cast<uint32_t>(maxInvincibleTime);
		ImGui::Checkbox("onGround_", &canFirstJump_);
		ImGui::Checkbox("canSecondJump_", &canSecondJump_);
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/Player/Player.json");
			JSON_OBJECT("Player");
			JSON_SAVE(verticalSpeed_);
			JSON_SAVE(horizontalSpeed_);
			JSON_SAVE(jumpPower_);
			JSON_SAVE(gravity_);
			JSON_SAVE(chaseLimitLine_);
			JSON_SAVE(runAwayLimitLine_);
			JSON_SAVE(knockBack_);
			JSON_SAVE(maxInvincibleTime_);
			JSON_SAVE(offset_);
			JSON_SAVE(revengeStartOffset_);
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}