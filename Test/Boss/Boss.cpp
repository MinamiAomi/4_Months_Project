#include "Boss.h"

#include "CharacterState.h"
#include "CollisionAttribute.h"
#include "Framework/ResourceManager.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"
#include "GameSpeed.h"
#include "Player/Player.h"
#include "CameraManager/CameraManager.h"
#include "Math/Camera.h"
#include "Movie.h"
#include "BossBulletManager.h"


void Boss::Initialize() {
#pragma endregion
	JSON_OPEN("Resources/Data/Boss/Boss.json");
	JSON_OBJECT("Boss");
	JSON_LOAD(offset_);
	JSON_ROOT();
	JSON_CLOSE();

	state_ = std::make_unique<BossStateManager>(*this);
	state_->Initialize();
	state_->ChangeState(BossStateManager::State::kRoot);

	bossHP_ = std::make_unique<BossHP>();
	bossHP_->SetCamera(camera_);
	bossHP_->Initialize();
	
	bossUI_ = std::make_unique<BossUI>();
	bossUI_->SetBossHP(bossHP_.get());
	bossUI_->Initialize();


	bossModelManager_ = std::make_unique<BossModelManager>();
	bossModelManager_->Initialize(&transform, player_);

	
	BossBulletManager::GetInstance()->Initialize();

	Reset(0);
	// 隠す
	/*bossModelManager_->GetModel(BossParts::kFloorAll)->SetIsAlive(false);
	bossModelManager_->GetModel(BossParts::kLongDistanceAttack)->SetIsAlive(false);*/
	isMove_ = true;
	isEndFirstHitMovie_ = false;

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Boss");
	collider_->SetCenter(Vector3( transform.translate.x, transform.translate.y, transform.translate.z-10.0f));
	collider_->SetOrientation(transform.rotate);
	// 鉾方向にくっそでかく（プレイヤーの弾がうしろにいかないよう
	Vector3 modelSize = (bossModelManager_->GetModel(BossParts::kBossBody)->GetModel()->GetModel()->GetMeshes().at(0).maxVertex - bossModelManager_->GetModel(BossParts::kBossBody)->GetModel()->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * 2.0f,modelSize.y ,modelSize.z*0.8f});
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Boss);
	collider_->SetCollisionMask(CollisionAttribute::Player | CollisionAttribute::DropGimmickDropperBall | CollisionAttribute::BossAttackTrigger);
	collider_->SetIsActive(true);

#pragma endregion

	isFirstHit_ = false;
	isHit_ = false;
}

void Boss::Update() {
	isHit_ = false;
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Boss")) {
		ImGui::DragFloat3("Pos", &transform.translate.x);
		ImGui::DragFloat4("Rotate", &transform.rotate.x);
		ImGui::DragFloat3("offset_", &offset_.x);
		toCameraVector_ = camera_->GetPosition() - transform.worldMatrix.GetTranslate();
		ImGui::Text("boss to camera Vector3  %f,%f,%f", toCameraVector_.x, toCameraVector_.y, toCameraVector_.z);
		if (ImGui::Button("OffsetSave")) {
			JSON_OPEN("Resources/Data/Boss/Boss.json");
			JSON_OBJECT("Boss");
			JSON_SAVE(offset_);
			JSON_ROOT();
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
	if (Character::IsInSceneChange()) {
		easingStartPosition_ = transform.translate;
	}
	
	switch (Character::currentCharacterState_) {
	case Character::State::kChase:
	{
		if (Character::IsOutSceneChange()) {
			transform.rotate = Quaternion::MakeForYAxis(180.0f * Math::ToRadian);
		}
		if (isMove_) {
			transform.translate.z += GameSpeed::GetGameSpeed();
		}
	}
	break;
	case Character::State::kRunAway:
	{
		if (Character::IsOutSceneChange()) {
			transform.rotate = Quaternion::MakeForYAxis(0.0f * Math::ToRadian);
		}
		if (isMove_) {
			transform.translate.z -= GameSpeed::GetGameSpeed();
		}
	}
	break;
	case Character::State::kScneChange:
	{
		BossBulletManager::GetInstance()->Reset();
		if (Character::isEndFirstChange_) {
			if (Character::nextCharacterState_ == Character::State::kChase) {
				transform.translate.z = std::lerp(easingStartPosition_.z, player_->transform.worldMatrix.GetTranslate().z + player_->GetChaseLimitLine(), Character::GetSceneChangeTime());
				if (transform.rotate != Quaternion::MakeForYAxis(180.0f * Math::ToRadian)) {
					transform.rotate = Quaternion::Slerp(Character::GetSceneChangeTime(), Quaternion::MakeForYAxis(0.0f * Math::ToRadian), Quaternion::MakeForYAxis(180.0f * Math::ToRadian));
				}


			}
			else {
				if (player_->transform.translate.z <= transform.translate.z - player_->GetRunAwayLimitLine()) {
					float tmp = (transform.translate.z - player_->GetRunAwayLimitLine()) - player_->transform.translate.z;
					transform.translate.z -= tmp;
				}
				if (transform.rotate != Quaternion::MakeForYAxis(0.0f * Math::ToRadian)) {
					transform.rotate = Quaternion::Slerp(Character::GetSceneChangeTime(), Quaternion::MakeForYAxis(180.0f * Math::ToRadian), Quaternion::MakeForYAxis(0.0f * Math::ToRadian));

				}
			}
		}
	}
	break;
	default:
		break;
	}
	state_->Update();
	bossUI_->Update();
	bossHP_->Update();
	BossBulletManager::GetInstance()->Update();
	UpdateTransform();
	if (bossHP_->GetCurrentHP() <= 0) {
		isAlive_ = false;
	}
}

void Boss::Reset(uint32_t stageIndex) {
	stageIndex;
	isAlive_ = true;
	isFirstHit_ = false;
	// ボスのオフセット今はプレイヤーのチェイスライン
	transform.translate = Vector3(offset_.x, offset_.y, player_->transform.worldMatrix.GetTranslate().z + player_->GetChaseLimitLine());
	transform.rotate = Quaternion::MakeForYAxis(180.0f * Math::ToRadian);

	transform.scale = Vector3::one;
	transform.UpdateMatrix();
	state_->ChangeState(BossStateManager::State::kRoot);
	bossHP_->Reset();
	bossModelManager_->Reset();
	BossBulletManager::GetInstance()->Reset();
}

void Boss::UpdateTransform() {
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetOrientation(rotate);

	switch (Character::currentCharacterState_) {
	case Character::State::kChase:
	{
		collider_->SetCenter(Vector3(transform.translate.x, transform.translate.y, transform.translate.z - 10.0f));
	}
	break;
	case Character::State::kRunAway:
	{
		collider_->SetCenter(Vector3(transform.translate.x, transform.translate.y, transform.translate.z ));
	}
	break;
	default:
		break;
	}
	bossModelManager_->Update();
}

void Boss::OnCollision(const CollisionInfo& collisionInfo) {
	state_->OnCollision(collisionInfo);
	if (collisionInfo.collider->GetName() == "Player") {

		switch (Character::currentCharacterState_) {
		case Character::State::kChase:
		{
			isHit_ = true;
			player_->GetRevengeGage()->SetCurrentRevengeBarGage(0.0f);
			if (isFirstHit_ && !Movie::isPlaying) {
				bossHP_->AddPlayerHitHP();
			}

		}
		break;
		case Character::State::kRunAway:
		{

		}
		break;
		default:
			break;
		}
		//一回目
		
	}

	//if (collisionInfo.collider->GetName() == "DropGimmickBall") {
	//	switch (Character::currentCharacterState_) {
	//	case Character::State::kChase:
	//	{
	//		bossHP_->AddBallHitHP();
	//		if (bossHP_ < 0) {
	//			isAlive_ = false;
	//		}
	//	}
	//	break;
	//	case Character::State::kRunAway:
	//	{

	//	}
	//	break;
	//	default:
	//		break;
	//	}
	//}
}
