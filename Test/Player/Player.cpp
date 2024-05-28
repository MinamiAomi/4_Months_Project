#include "Player.h"

#include "Engine/Graphics/RenderManager.h"
#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Graphics/ImGuiManager.h"
#include "File/JsonHelper.h"
#include "FollowCamera.h"
#include "Framework/ResourceManager.h"
#include "Input/Input.h"
#include "Trap/TrapManager.h"
#include "Collision/CollisionManager.h"
#include "Movie.h"

void Player::Initialize() {
#pragma region パラメーター
	JSON_OPEN("Resources/Data/Player/Player.json");
	JSON_OBJECT("Player");
	JSON_LOAD(verticalSpeed_);
	JSON_LOAD(horizontalSpeed_);
	JSON_LOAD(jumpPower_);
	JSON_LOAD(dashPower_);
	JSON_LOAD(gravity_);
	JSON_LOAD(chaseLimitLine_);
	JSON_LOAD(runAwayLimitLine_);
	JSON_LOAD(knockBack_);
	JSON_LOAD(maxInvincibleTime_);
	JSON_LOAD(dashMaxCount_);
	JSON_LOAD(dashIntervalCount_);
	JSON_LOAD(offset_);
	JSON_LOAD(hitJump_);
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
	playerRevengeGage_->Initialize(boss_);

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

	// アニメーション
	animation_ = ResourceManager::GetInstance()->FindAnimation("player");
	skeleton_ = std::make_shared<Skeleton>();
	skeleton_->Create(model_->GetModel());
	animationTime_ = 0.0f;
	skeleton_->ApplyAnimation(animation_->GetAnimation("idle"), 0.0f);
	model_->SetSkeleton(skeleton_);
	animationType_ = IdleAnimation;

	hammer_ = std::make_unique<PlayerHammer>();
	hammer_->Initialize(this);

	//playerModel_.Initialize(&transform);
	//playerModel_.PlayAnimation(PlayerModel::kWait, true);
}

void Player::Update() {

	switch (Character::currentCharacterState_) {
	case Character::State::kChase:
	case Character::State::kRunAway:
		//false
		preIsHit_ = isHit_;
		isGround_ = false;
		isMove_ = false;

		if (playerHP_->GetCurrentHP() <= 0) {
			isAlive_ = false;
		}
		if (!isHit_ && !isDash_) {
			// 移動
			Move();

			// ジャンプ
			Jump();

			// ダッシュ
			//Dash();

			//SetTrap();
		}
		else {
			velocity_ = Vector3::zero;
			if (isDash_) {
				dashCount_--;
				if (dashCount_ <= 0) {
					dashVector_ = Vector3::zero;
					isDash_ = false;
				}
			}
			if (isHit_) {
				Vector3 rotate = { 0.0f,rnd_.NextFloatUnit() ,0.0f };
				transform.rotate = Quaternion::MakeFromEulerAngle(rotate);
			}
		}
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
			(playerRevengeGage_->GetCurrentRevengeBarGage() >= PlayerRevengeGage::kMaxRevengeBar)) {
			Character::SetNextScene(Character::State::kChase);
			//transform.translate.x = 0.0f;
			//transform.translate.z = boss_->transform.worldMatrix.GetTranslate().z - chaseLimitLine_;
			transform.rotate = Quaternion::identity;
			canFirstJump_ = true;
			canSecondJump_ = true;
			velocity_ = Vector3::zero;
			acceleration_ = Vector3::zero;
			revengeSE_->SetVolume(0.1f);
			revengeSE_->Play();
		}

		if (Character::currentCharacterState_ == Character::State::kChase &&
			playerRevengeGage_->GetCurrentRevengeBarGage() <= 0) {
			Character::SetNextScene(Character::State::kRunAway);
		}
		if (!isDash_) {
			acceleration_.y += gravity_;
		}
		else {
			acceleration_.y = 0.0f;
		}
		acceleration_.z *= 0.9f;
		velocity_ += acceleration_;
		if (!isHit_) {
			Vector3 dashVelocity = dashVector_ * dashPower_;
			transform.translate += dashVelocity;
		}
		transform.translate += velocity_ + Vector3(0.0f, 0.0f, beltConveyorVelocity_);

		beltConveyorVelocity_ = 0.0f;

		transform.translate.x = std::clamp(transform.translate.x, -20.0f, 20.0f);
		transform.translate.z = std::min(transform.translate.z, boss_->transform.translate.z - 10.0f);
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

		case Character::State::kScneChange:
			// はいらない
			break;
		default:
			break;
		}
	}
	DebugParam();
	UpdateTransform();
	AnimationUpdate();

	// 弾アップデート
	//bulletManager_->Update(transform.worldMatrix.GetTranslate());

	//playerModel_.Update();
}

void Player::AnimationUpdate() {

	animationTime_ += 1.0f / (animationTransition_ ? 20.0f : 60.0f);
	if (animationTime_ > 1.0f) {
		if (animationTransition_) {
			animationTransition_ = false;
		}
		animationTime_ -= 1.0f;
	}

	switch (animationType_) {
	case Player::IdleAnimation:
		if (isMove_) {
			fromTransitionAnimationTime_ = animationTime_;
			animationType_ = MoveAnimation;
			animationTime_ = 0.0f;
			skeleton_->ApplyAnimationTransition(animation_->GetAnimation("idle"), fromTransitionAnimationTime_, animation_->GetAnimation("move"), 0.0f, animationTime_);
			animationTransition_ = true;
		}
		else {
			if (animationTransition_) {
				skeleton_->ApplyAnimationTransition(animation_->GetAnimation("move"), fromTransitionAnimationTime_, animation_->GetAnimation("idle"), 0.0f, animationTime_);
			}
			else {
				skeleton_->ApplyAnimation(animation_->GetAnimation("idle"), animationTime_);
			}
		}
		break;
	case Player::MoveAnimation:
		if (!isMove_) {
			fromTransitionAnimationTime_ = animationTime_;
			animationType_ = IdleAnimation;
			animationTime_ = 0.0f;
			skeleton_->ApplyAnimationTransition(animation_->GetAnimation("move"), fromTransitionAnimationTime_, animation_->GetAnimation("idle"), 0.0f, animationTime_);
			animationTransition_ = true;
		}
		else {
			if (animationTransition_) {
				skeleton_->ApplyAnimationTransition(animation_->GetAnimation("idle"), fromTransitionAnimationTime_, animation_->GetAnimation("move"), 0.0f, animationTime_);
			}
			else {
				skeleton_->ApplyAnimation(animation_->GetAnimation("move"), animationTime_);
			}
		}
		break;
	}
	skeleton_->Update();
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
	isHit_ = false;
	isSceneChangeInvincible_ = false;
	preIsHit_ = isHit_;
	velocity_ = Vector3::zero;
	acceleration_ = Vector3::zero;
	dashVector_ = Vector3::zero;
	beltConveyorVelocity_ = 0.0f;
	isDash_ = false;
	playerHP_->Reset();
	playerRevengeGage_->Reset();
}

void Player::HitDamage(uint32_t damage) {
	if (!isSceneChangeInvincible_) {
		isHit_ = true;
	}

	if (invincibleTime_ == 0) {
		if (damage != (uint32_t)-1) {
			invincibleTime_ = maxInvincibleTime_;
			if (playerHP_->GetCurrentHP() > 0) {
				int tmp = damage;
				playerHP_->AddHP(-tmp);
			}
		}
		else {
			velocity_ = Vector3::zero;
			acceleration_.y = hitJump_;
			invincibleTime_ = maxInvincibleTime_;
			isHit_ = true;
		}
	}
}

void Player::UpdateTransform() {
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetOrientation(rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
	collider_->DebugDraw();
	hammer_->UpdateTransform();
}

void Player::SceneChangeUpdate() {
	if (Character::currentCharacterState_ == Character::kScneChange && Character::nextCharacterState_ == Character::kRunAway) {
		isSceneChangeInvincible_ = true;
		invincibleTime_ = maxInvincibleTime_;
	}
}

void Player::OnCollision(const CollisionInfo& collisionInfo) {

	if (!Movie::isPlaying) {
		if (collisionInfo.collider->GetName() == "Boss") {
			switch (Character::currentCharacterState_) {
			case Character::State::kChase:
			{
				transform.translate.z -= 5.0f;
			}
			break;
			case Character::State::kRunAway:
			{
				if (!isHit_ && !isSceneChangeInvincible_) {
					acceleration_.z -= knockBack_;
				}
				HitDamage(1);
			}
			break;
			default:
				break;
			}
		}
		else if (collisionInfo.collider->GetName() == "Block" ||
			collisionInfo.collider->GetName() == "FireBarCenter" ||
			collisionInfo.collider->GetName() == "Floor" ||
			collisionInfo.collider->GetName() == "StageObject" ||
			collisionInfo.collider->GetName() == "BeltConveyor" ||
			collisionInfo.collider->GetName() == "DropGimmickDropper" ||
			collisionInfo.collider->GetName() == "DropGimmickSwitch") {
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
				if (preIsHit_ && isHit_) {
					isHit_ = false;
				}
			}

			UpdateTransform();
			//const GameObject* nextParent = collisionInfo.collider->GetGameObject();
			//if (nextParent) {
			//	transform.SetParent(&nextParent->transform);
			//}
		}
		else if (collisionInfo.collider->GetName() == "bossLeftHand" ||
			collisionInfo.collider->GetName() == "bossFloorAll" ||
			collisionInfo.collider->GetName() == "bossLongDistanceAttack") {
			HitDamage(1);
		}
		else if ((collisionInfo.collider->GetName() == "FireBarBar" ||
			collisionInfo.collider->GetName() == "PendulumBall") &&
			!isHit_) {
			HitDamage();
		}
		else if (collisionInfo.collider->GetName() == "RevengeCoin") {
			playerRevengeGage_->AddGage();
		}

		//RayCastInfo rayCastInfo{};
		//if (CollisionManager::GetInstance()->RayCast(transform.worldMatrix.GetTranslate(), transform.worldMatrix.GetTranslate() + Vector3(50.0f, 0.0f, 0.0f), ~CollisionAttribute::Player, &rayCastInfo)) {
		//	RenderManager::GetInstance()->GetLineDrawer().AddLine(transform.worldMatrix.GetTranslate(), transform.worldMatrix.GetTranslate() + Vector3(50.0f, 0.0f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f));

		//}
		//else {

		//	RenderManager::GetInstance()->GetLineDrawer().AddLine(transform.worldMatrix.GetTranslate(), transform.worldMatrix.GetTranslate() + Vector3(50.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f));
		//}
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
		jumpSE_->SetVolume(0.1f);
		acceleration_.y += jumpPower_;
		canFirstJump_ = false;
	}
	else if (!canFirstJump_ &&
		canSecondJump_ &&
		(Input::GetInstance()->IsKeyTrigger(DIK_SPACE) ||
			((Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
				!(Input::GetInstance()->GetPreXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_A)))) {
		jumpSE_->Play();
		jumpSE_->SetVolume(0.1f);
		canSecondJump_ = false;
		acceleration_.y = jumpPower_ * 0.5f;
	}
}

void Player::Dash() {
	if ((Input::GetInstance()->IsKeyTrigger(DIK_LSHIFT) ||
		((Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
			!(Input::GetInstance()->GetPreXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_B))) &&
		dashCoolTime_ <= 0) {
		dashVector_ = transform.rotate.GetForward();
		dashVector_ = dashVector_.Normalize();
		isDash_ = true;
		dashCount_ = dashMaxCount_;
		dashCoolTime_ = dashIntervalCount_;
		velocity_ = Vector3::zero;
		acceleration_ = Vector3::zero;
	}
	else {
		if (dashCoolTime_ > 0) {
			dashCoolTime_--;
		}
		dashCoolTime_ = std::clamp(dashCoolTime_, 0u, dashIntervalCount_);
		dashVector_ = Vector3::zero;
	}
}

void Player::Invincible() {
	if (invincibleTime_ > 0) {
		if (!isSceneChangeInvincible_) {
			model_->SetColor({ 1.0f,0.0f,0.0f });
		}
		invincibleTime_--;
		if (invincibleTime_ == 0) {
			isSceneChangeInvincible_ = false;
			isHit_ = false;
			model_->SetColor({ 1.0f,1.0f,1.0f });
		}
	}
	else {
		isSceneChangeInvincible_ = false;
		isHit_ = false;
		model_->SetColor({ 1.0f,1.0f,1.0f });
	}
}

void Player::SetTrap() {

	if (Character::currentCharacterState_ == Character::State::kRunAway &&
		(Input::GetInstance()->IsKeyTrigger(DIK_J) ||
			((Input::GetInstance()->GetXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
				!(Input::GetInstance()->GetPreXInputState().Gamepad.wButtons & XINPUT_GAMEPAD_B)))) {
		trapManager_->Create(transform.worldMatrix.GetTranslate());
	}

}

void Player::DebugParam() {
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Player")) {
		ImGui::DragFloat3("Position", &transform.translate.x);
		ImGui::DragFloat4("Rotate", &transform.rotate.x);
		ImGui::DragFloat3("velocity", &velocity_.x);
		ImGui::DragFloat3("acceleration", &acceleration_.x);
		int time = invincibleTime_;
		ImGui::DragInt("invincibleTime_", &time);
		invincibleTime_ = time;
		ImGui::Checkbox("onGround_", &canFirstJump_);
		ImGui::Checkbox("canSecondJump_", &canSecondJump_);
		ImGui::Checkbox("isHit_", &isHit_);
		ImGui::Checkbox("preIsHit_", &preIsHit_);
		ImGui::Checkbox("isSceneChangeInvincible_", &isSceneChangeInvincible_);
		if (ImGui::TreeNode("パラメーター")) {
			ImGui::DragFloat3("オフセット", &offset_.x);
			ImGui::DragFloat("縦のスピード", &verticalSpeed_);
			ImGui::DragFloat("横のスピード", &horizontalSpeed_);
			ImGui::DragFloat("ジャンプ力", &jumpPower_);
			ImGui::DragFloat("ダッシュ力", &dashPower_);
			ImGui::DragFloat("ヒット時のジャンプ", &hitJump_);
			ImGui::DragFloat("重力", &gravity_);
			ImGui::DragFloat("復讐時リミットライン", &chaseLimitLine_);
			ImGui::DragFloat("逃げてる時のリミットライン", &runAwayLimitLine_);
			ImGui::DragFloat("knockBack_", &knockBack_);
			int maxInvincibleTime = maxInvincibleTime_;
			ImGui::DragInt("無敵時間", &maxInvincibleTime);
			maxInvincibleTime_ = static_cast<uint32_t>(maxInvincibleTime);
			int dashMaxCount = dashMaxCount_;
			ImGui::DragInt("ダッシュ時間", &dashMaxCount);
			dashMaxCount_ = static_cast<uint32_t>(dashMaxCount);
			int dashIntervalCount = dashIntervalCount_;
			ImGui::DragInt("ダッシュクールタイム", &dashIntervalCount);
			dashIntervalCount_ = static_cast<uint32_t>(dashIntervalCount);
			if (ImGui::Button("Save")) {
				JSON_OPEN("Resources/Data/Player/Player.json");
				JSON_OBJECT("Player");
				JSON_SAVE(verticalSpeed_);
				JSON_SAVE(horizontalSpeed_);
				JSON_SAVE(jumpPower_);
				JSON_SAVE(dashPower_);
				JSON_SAVE(gravity_);
				JSON_SAVE(chaseLimitLine_);
				JSON_SAVE(runAwayLimitLine_);
				JSON_SAVE(knockBack_);
				JSON_SAVE(maxInvincibleTime_);
				JSON_SAVE(dashMaxCount_);
				JSON_SAVE(dashIntervalCount_);
				JSON_SAVE(offset_);
				JSON_SAVE(hitJump_);
				JSON_CLOSE();
			}
			ImGui::TreePop();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}