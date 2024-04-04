#include "Player.h"

#include <fstream>

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

	//playerModel_.Initialize(&transform);
	//playerModel_.PlayAnimation(PlayerModel::kWait, true);

#pragma region パラメーター
	LoadParameter(defaultSpeed_, "defaultSpeed_");
	LoadParameter(verticalSpeed_, "verticalSpeed_");
	LoadParameter(horizontalSpeed_, "horizontalSpeed_");
#pragma endregion

}

void Player::Update() {
	Move();

	DebugParam();

	transform.UpdateMatrix();
	//playerModel_.Update();
	model_->SetWorldMatrix(transform.worldMatrix);
}

void Player::Move() {
	auto input = Input::GetInstance();
	auto followCamera = followCamera_.lock();

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

	// 移動処理
	if (move != Vector3::zero) {
		move = move.Normalized();
		// 地面に水平なカメラの回転
		if (followCamera) {
			move = followCamera->GetCamera()->GetRotate() * move;
		}
		move.y = 0.0f;
		move = move.Normalized();

		move.x *= horizontalSpeed_;
		move.z *= verticalSpeed_;

		// 親がいる場合親の空間にする
		const Transform* parent = transform.GetParent();
		if (parent) {
			move = parent->worldMatrix.Inverse().ApplyRotation(move);
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
	// 移動
	// 強制敵に進む
	move.z += defaultSpeed_;
	transform.translate += move;
	// 回転
	transform.rotate = Quaternion::Slerp(0.2f, transform.rotate, Quaternion::MakeLookRotation(move));

	move = transform.rotate.Conjugate() * move;
	Quaternion diff = Quaternion::MakeFromTwoVector(Vector3::unitZ, move);
	transform.rotate = Quaternion::Slerp(0.2f, Quaternion::identity, diff) * transform.rotate;
}

void Player::DebugParam() {
#ifdef _DEBUG
	ImGui::Begin(fileName_.c_str());
	ImGui::DragFloat("defaultSpeed_",&defaultSpeed_);
	ImGui::DragFloat("verticalSpeed_",&verticalSpeed_);
	ImGui::DragFloat("horizontalSpeed_",&horizontalSpeed_);
	if (ImGui::Button("Save")) {
		SaveParameter(defaultSpeed_,"defaultSpeed_");
		SaveParameter(verticalSpeed_,"verticalSpeed_");
		SaveParameter(horizontalSpeed_,"horizontalSpeed_");
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