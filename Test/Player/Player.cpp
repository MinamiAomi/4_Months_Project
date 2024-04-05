#include "Player.h"

#include <fstream>

#include "CollisionAttribute.h"
#include "Externals/nlohmann/json.hpp"
#include "Externals/ImGui/imgui.h"
#include "FollowCamera.h"
#include "Graphics/ResourceManager.h"
#include "Input/Input.h"

void Player::Initialize() {

	fileName_ = "Player";

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
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
	collider_->SetCollisionAttribute(CollisionAttribute::Player);
	collider_->SetCollisionMask(~CollisionAttribute::Player);
	//collider_->SetIsActive(false);
#pragma endregion

	//playerModel_.Initialize(&transform);
	//playerModel_.PlayAnimation(PlayerModel::kWait, true);

#pragma region パラメーター
	LoadParameter(defaultSpeed_, "defaultSpeed_");
	LoadParameter(verticalSpeed_, "verticalSpeed_");
	LoadParameter(horizontalSpeed_, "horizontalSpeed_");
	LoadParameter(jumpPower_, "jumpPower_");
	LoadParameter(gravity_, "gravity_");
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

void Player::OnCollision(const CollisionInfo& collisionInfo) {
	collisionInfo;
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
		transform.rotate = Quaternion::Slerp(0.2f, transform.rotate, Quaternion::MakeLookRotation(move));

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
	ImGui::Begin(fileName_.c_str());
	ImGui::DragFloat3("Pos", &transform.translate.x);
	ImGui::DragFloat3("velocity_", &velocity_.x);
	ImGui::DragFloat("defaultSpeed_", &defaultSpeed_);
	ImGui::DragFloat("verticalSpeed_", &verticalSpeed_);
	ImGui::DragFloat("horizontalSpeed_", &horizontalSpeed_);
	ImGui::DragFloat("jumpPower_", &jumpPower_);
	ImGui::DragFloat("gravity_", &gravity_);
	if (ImGui::Button("Save")) {
		SaveParameter(defaultSpeed_, "defaultSpeed_");
		SaveParameter(verticalSpeed_, "verticalSpeed_");
		SaveParameter(horizontalSpeed_, "horizontalSpeed_");
		SaveParameter(jumpPower_, "jumpPower_");
		SaveParameter(gravity_, "gravity_");
	}
	ImGui::End();
#endif // _DEBUG
}

void Player::SaveJson() {

}

#pragma region Json
void Player::SaveParameter(float param, std::string name) {
	std::string filePath = "Resources/Data/" + fileName_ + "/" + fileName_ + ".json";

	nlohmann::json root;

	// 既存のファイルがあれば読み込み、なければ新しいJSONオブジェクトを作成
	if (std::filesystem::exists(filePath)) {
		std::ifstream inputFile(filePath);
		inputFile >> root;
		inputFile.close();
	}

	// 更新されたデータをJSONオブジェクトに追加
	root[fileName_][name] = param;

	// ディレクトリが存在しない場合は作成
	const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_;
	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}

	// JSONファイルに書き込み
	std::ofstream outputFile(filePath);
	outputFile << std::setw(4) << root << std::endl;
	outputFile.close();
}

void Player::LoadParameter(float& param, std::string name) {
	const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_ + "/" + fileName_;
	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath.string() + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ifs.fail()) {
		MessageBox(nullptr, L"指定したファイルは存在しません。", L"Editor - Load", 0);
		return;
	}
	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(fileName_);
	//未登録チェック
	if (itGroup == root.end()) {
		MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Editor - Load", 0);
	}
	// アイテム
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		//グループを検索
		nlohmann::json::iterator itObject = itGroup->find(itemName);

		//未登録チェック
		if (itObject == itGroup->end()) {
			MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Editor - Load", 0);
		}

		//保険
		assert(itObject != itGroup->end());//アイテム名がオブジェクトデータだった場合、登録

		if (itemName == name) {
			param = itItem->get<float>();
		}
	}
}

void Player::SaveParameter(const Vector2& param, std::string name) {
	nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	root[fileName_][name] = nlohmann::json::array({ param.x,param.y });


	const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_ + "/" + fileName_;

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}
	std::string filePath = kDirectoryPath.string() + std::string(".json");

	std::ofstream file(filePath);

	if (file.fail()) {
		assert(!"fileSaveFailed");
		return;
	}

	file << std::setw(4) << root << std::endl;

	file.close();
}

void Player::LoadParameter(Vector2& param, std::string name) {
	const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_ + "/" + fileName_;
	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath.string() + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ifs.fail()) {
		MessageBox(nullptr, L"指定したファイルは存在しません。", L"Editor - Load", 0);
		return;
	}
	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(fileName_);
	//未登録チェック
	if (itGroup == root.end()) {
		MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Editor - Load", 0);
	}
	// アイテム
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		//グループを検索
		nlohmann::json::iterator itObject = itGroup->find(itemName);

		//未登録チェック
		if (itObject == itGroup->end()) {
			MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Editor - Load", 0);
		}

		//保険
		assert(itObject != itGroup->end());//アイテム名がオブジェクトデータだった場合、登録

		if (itemName == name) {
			param = Vector2(itItem->at(0), itItem->at(1));
		}
	}
}

void Player::SaveParameter(const Vector3& param, std::string name) {
	nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	root[fileName_][name] = nlohmann::json::array({ param.x,param.y, param.z });


	const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_ + "/" + fileName_;

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}
	std::string filePath = kDirectoryPath.string() + std::string(".json");

	std::ofstream file(filePath);

	if (file.fail()) {
		assert(!"fileSaveFailed");
		return;
	}

	file << std::setw(4) << root << std::endl;

	file.close();
}

void Player::LoadParameter(Vector3& param, std::string name) {
	const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_ + "/" + fileName_;
	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath.string() + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ifs.fail()) {
		MessageBox(nullptr, L"指定したファイルは存在しません。", L"Editor - Load", 0);
		return;
	}
	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(fileName_);
	//未登録チェック
	if (itGroup == root.end()) {
		MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Editor - Load", 0);
	}
	// アイテム
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		//グループを検索
		nlohmann::json::iterator itObject = itGroup->find(itemName);

		//未登録チェック
		if (itObject == itGroup->end()) {
			MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Editor - Load", 0);
		}

		//保険
		assert(itObject != itGroup->end());//アイテム名がオブジェクトデータだった場合、登録

		if (itemName == name) {
			param = Vector3(itItem->at(0), itItem->at(1), itItem->at(2));
		}
	}
}

void Player::SaveParameter(const Vector4& param, std::string name) {
	nlohmann::json root;

	root = nlohmann::json::object();

	root[fileName_] = nlohmann::json::object();

	root[fileName_][name] = nlohmann::json::array({ param.x,param.y, param.z,param.w });


	const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_ + "/" + fileName_;

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}
	std::string filePath = kDirectoryPath.string() + std::string(".json");

	std::ofstream file(filePath);

	if (file.fail()) {
		assert(!"fileSaveFailed");
		return;
	}

	file << std::setw(4) << root << std::endl;

	file.close();
}

void Player::LoadParameter(Vector4& param, std::string name) {
	const std::filesystem::path kDirectoryPath = "Resources/Data/" + fileName_ + "/" + fileName_;
	//読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath.string() + ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープン失敗したら表示
	if (ifs.fail()) {
		MessageBox(nullptr, L"指定したファイルは存在しません。", L"Editor - Load", 0);
		return;
	}
	nlohmann::json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();
	//グループを検索
	nlohmann::json::iterator itGroup = root.find(fileName_);
	//未登録チェック
	if (itGroup == root.end()) {
		MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Editor - Load", 0);
	}
	// アイテム
	for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		//グループを検索
		nlohmann::json::iterator itObject = itGroup->find(itemName);

		//未登録チェック
		if (itObject == itGroup->end()) {
			MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Editor - Load", 0);
		}

		//保険
		assert(itObject != itGroup->end());//アイテム名がオブジェクトデータだった場合、登録

		if (itemName == name) {
			param = Vector4(itItem->at(0), itItem->at(1), itItem->at(2), itItem->at(3));
		}
	}
}
#pragma endregion