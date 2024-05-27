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
	bossHP_->Initialize();

	bossUI_ = std::make_unique<BossUI>();
	bossUI_->SetBossHP(bossHP_.get());
	bossUI_->Initialize();

	Reset(0);

	bossModelManager_ = std::make_unique<BossModelManager>();
	bossModelManager_->Initialize(&transform, player_);

	// 隠す
	/*bossModelManager_->GetModel(BossParts::kFloorAll)->SetIsAlive(false);
	bossModelManager_->GetModel(BossParts::kLongDistanceAttack)->SetIsAlive(false);*/
	isMove_ = true;

#pragma region コライダー
	collider_ = std::make_unique<BoxCollider>();
	collider_->SetGameObject(this);
	collider_->SetName("Boss");
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	// 鉾方向にくっそでかく（プレイヤーの弾がうしろにいかないよう
	Vector3 modelSize = (bossModelManager_->GetModel(BossParts::kBossBody)->GetModel()->GetModel()->GetMeshes().at(0).maxVertex - bossModelManager_->GetModel(BossParts::kBossBody)->GetModel()->GetModel()->GetMeshes().at(0).minVertex);
	collider_->SetSize({ modelSize.x * 2.0f,modelSize.y ,modelSize.z + 5.0f});
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Boss);
	collider_->SetCollisionMask(~CollisionAttribute::Boss);
	collider_->SetIsActive(true);


#pragma endregion

	isFirstHit_ = false;
}

void Boss::Update() {
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
	break;
	default:
		break;
	}
	UpdateTransform();
	state_->Update();
	bossUI_->Update();
	bossHP_->Update();
	bossModelManager_->Update();
	if (bossHP_->GetCurrentHP() <= 0) {
		isAlive_ = false;
	}
}

void Boss::Reset(uint32_t stageIndex) {
	stageIndex;
	isAlive_ = true;
	// ボスのオフセット今はプレイヤーのチェイスライン
	transform.translate = Vector3(offset_.x, offset_.y, player_->transform.worldMatrix.GetTranslate().z + player_->GetChaseLimitLine());
	transform.rotate = Quaternion::MakeForYAxis(180.0f * Math::ToRadian);

	transform.scale = Vector3::one;
	transform.UpdateMatrix();
	state_->ChangeState(BossStateManager::State::kRoot);
	bossHP_->Reset();

}

void Boss::UpdateTransform() {
	transform.UpdateMatrix();
	Vector3 scale, translate;
	Quaternion rotate;
	transform.worldMatrix.GetAffineValue(scale, rotate, translate);
	collider_->SetCenter(translate);
	collider_->SetOrientation(rotate);
	collider_->DebugDraw();
}

void Boss::OnCollision(const CollisionInfo& collisionInfo) {
	state_->OnCollision(collisionInfo);
	if (collisionInfo.collider->GetName() == "Player") {

		switch (Character::currentCharacterState_) {
		case Character::State::kChase:
		{
			////二回目でゲームクリア
			//if (isFirstHit_ == true) {
			//	isAlive_ = false;
			//}
			bossHP_->AddPlayerHitHP();
			Character::SetNextScene(Character::State::kRunAway);

		}
		break;
		case Character::State::kRunAway:
		{

		}
		break;
		default:
			break;
		}
		////一回目
		//isFirstHit_ = true;
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
