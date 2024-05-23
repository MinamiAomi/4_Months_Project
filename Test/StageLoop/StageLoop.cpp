#include "StageLoop.h"

#include <filesystem>
#include <fstream>
#include <regex>

#include "Boss/Boss.h"
#include "CameraManager/CameraManager.h"
#include "Player/Player.h"
#include "CharacterState.h"


void StageLoop::Initialize() {

	bossAttackTriggerManager_ = std::make_unique<BossAttackTriggerManager>();
	blockManager_ = std::make_unique<BlockManager>();
	beltConveyorManager_ = std::make_unique<BeltConveyorManager>();
	fireBarManager_ = std::make_unique<FireBarManager>();
	floorManager_ = std::make_unique<FloorManager>();
	dropGimmickManager_ = std::make_unique<DropGimmickManager>();
	pendulumManager_ = std::make_unique<PendulumManager>();
	revengeCoinManager_ = std::make_unique<RevengeCoinManager>();
	stageObjectManager_ = std::make_unique<StageObjectManager>();
	//trapManager_ = std::make_unique<TrapManager>();

	bossAttackTriggerManager_->SetCamera(camera_);
	bossAttackTriggerManager_->SetBoss(boss_);
	beltConveyorManager_->SetCamera(camera_);
	beltConveyorManager_->SetPlayer(player_);
	blockManager_->SetCamera(camera_);
	blockManager_->SetPlayer(player_);
	fireBarManager_->SetCamera(camera_);
	fireBarManager_->SetPlayer(player_);
	floorManager_->SetCamera(camera_);
	floorManager_->SetPlayer(player_);
	dropGimmickManager_->SetCamera(camera_);
	dropGimmickManager_->SetPlayer(player_);
	dropGimmickManager_->SetBoss(boss_);
	pendulumManager_->SetCamera(camera_);
	pendulumManager_->SetPlayer(player_);
	revengeCoinManager_->SetCamera(camera_);
	revengeCoinManager_->SetPlayer(player_);
	stageObjectManager_->SetCamera(camera_);
	stageObjectManager_->SetPlayer(player_);
	//trapManager_->SetCamera(camera_);
	//trapManager_->SetPlayer(player_);
	//trapManager_->SetBoss(boss_);
	//trapManager_->Initialize();

	LoadJson();

	InitializeCreateStage();

	isCreateStage_ = false;
}

void StageLoop::Update() {
	if (Character::currentCharacterState_ == Character::State::kScneChange &&
		Character::nextCharacterState_ == Character::State::kRunAway &&
		!isCreateStage_) {
		isCreateStage_ = true;
		CreateStage();
	}
	else if (Character::currentCharacterState_ == Character::State::kRunAway) {
		isCreateStage_ = false;
	}

	blockManager_->Update();
	fireBarManager_->Update();
	floorManager_->Update();
	dropGimmickManager_->Update();
	revengeCoinManager_->Update();
	pendulumManager_->Update();
	stageObjectManager_->Update();
	//trapManager_->Update();
	bossAttackTriggerManager_->Update();
	beltConveyorManager_->Update();
}

void StageLoop::Reset() {
	InitializeCreateStage();
}

void StageLoop::LoadJson() {
	//static std::string directoryPath = "Resources/Data/StageScene/debug.json";

	//Desc jsonData{};
	//std::vector<Switch::Desc> switchDesc{};
	//std::vector<Dropper::Desc> dropperDesc{};

	//std::ifstream ifs(directoryPath);
	//if (!ifs.is_open()) {
	//	return;
	//}
	//// JSONをパースしてルートオブジェクトを取得
	//nlohmann::json root;
	//ifs >> root;
	//ifs.close();
	//// "objects"配列からオブジェクトを処理
	//for (const auto& obj : root["objects"]) {
	//	// Block
	//	if (obj.contains("gimmick")) {
	//		if (obj["gimmick"]["type"] == "Block") {
	//			Block::Desc desc{};
	//			desc.desc = StageGimmick::GetDesc(obj);
	//			jsonData.blockDesc.emplace_back(desc);
	//		}
	//		else if (obj["gimmick"]["type"] == "Trigger") {
	//			BossAttackTrigger::Desc desc{};
	//			desc.desc = StageGimmick::GetDesc(obj);
	//			const auto& gimmick = obj["gimmick"];
	//			desc.state = static_cast<BossStateManager::State>(gimmick["state"] + 1);
	//			jsonData.bossAttackTrigger.emplace_back(desc);

	//		}
	//		// FireBar
	//		else if (obj["gimmick"]["type"] == "FireBar") {
	//			FireBar::Desc desc{};
	//			desc.desc = StageGimmick::GetDesc(obj);
	//			const auto& gimmick = obj["gimmick"];
	//			desc.barDesc.length = gimmick["length"];
	//			desc.barDesc.barInitialAngle = gimmick["initializeAngle"] * Math::ToRadian;
	//			desc.barDesc.rotateVelocity = gimmick["angularVelocity"] * Math::ToRadian;
	//			jsonData.fireBarDesc.emplace_back(desc);
	//		}
	//		// Floor
	//		else if (obj["gimmick"]["type"] == "Floor") {
	//			Floor::Desc desc{};
	//			desc.desc = StageGimmick::GetDesc(obj);
	//			jsonData.floorDesc.emplace_back(desc);
	//		}
	//		// Pendulam
	//		else if (obj["gimmick"]["type"] == "Pendulum") {
	//			Pendulum::Desc desc{};
	//			desc.desc = StageGimmick::GetDesc(obj);
	//			const auto& gimmick = obj["gimmick"];
	//			desc.length = gimmick["length"];
	//			desc.angle = gimmick["angle"] * Math::ToRadian;
	//			desc.initializeAngle = gimmick["initializeAngle"] * Math::ToRadian;
	//			desc.gravity = gimmick["gravity"];
	//			desc.stickScale = gimmick["stickScale"];
	//			desc.ballScale = gimmick["ballScale"];
	//			jsonData.pendulumDesc.emplace_back(desc);
	//		}
	//		// RevengeCoin
	//		else if (obj["gimmick"]["type"] == "RevengeCoin") {
	//			RevengeCoin::Desc desc{};
	//			desc.desc = StageGimmick::GetDesc(obj);
	//			jsonData.revengeCoinDesc.emplace_back(desc);
	//		}
	//		// BeltConveyor
	//		else if (obj["gimmick"]["type"] == "BeltConveyor") {
	//			BeltConveyor::Desc desc{};
	//			desc.desc = StageGimmick::GetDesc(obj);
	//			const auto& gimmick = obj["gimmick"];
	//			desc.velocity = gimmick["beltConveyorVelocity"];
	//			jsonData.beltConveyorDesc.emplace_back(desc);
	//		}
	//		// DroppGimmick
	//		else if (obj["gimmick"]["type"] == "DropGimmickSwitch") {
	//			Switch::Desc desc{};
	//			desc.desc = StageGimmick::GetDesc(obj);
	//			const auto& gimmick = obj["gimmick"];
	//			desc.index = gimmick["dropGimmickIndex"];
	//			switchDesc.emplace_back(desc);
	//		}
	//		else if (obj["gimmick"]["type"] == "DropGimmickDropper") {
	//			Dropper::Desc desc{};
	//			desc.desc = StageGimmick::GetDesc(obj);
	//			const auto& gimmick = obj["gimmick"];
	//			desc.index = gimmick["dropGimmickIndex"];
	//			dropperDesc.emplace_back(desc);
	//		}
	//	}
	//	// StageArea
	//	else if (obj["file_name"] == "stageArea") {
	//		if (obj.contains("collider")) {
	//			const auto& collider = obj["collider"];
	//			jsonData.stageSize = { collider["size"][2] };
	//		}

	//	}
	//	// StageObject
	//	else if (!obj.contains("gimmick")) {
	//		StageObject::Desc desc{};
	//		desc.desc = StageGimmick::GetDesc(obj);
	//		jsonData.stageObjectDesc.emplace_back(desc);
	//	}
	//}
	//// DropGimmickのソート
	//// １ステージにおけるスイッチのタイプの数
	//for (uint32_t i = 0; i < 10; i++) {
	//	DropGimmick::Desc dropGimmickDesc{};
	//	for (auto& desc : switchDesc) {
	//		if (desc.index == i) {
	//			dropGimmickDesc.switchDesc.emplace_back(desc);
	//		}
	//	}
	//	for (auto& desc : dropperDesc) {
	//		if (desc.index == i) {
	//			dropGimmickDesc.dropperDesc.emplace_back(desc);
	//		}
	//	}
	//	if (!dropGimmickDesc.switchDesc.empty() &&
	//		!dropGimmickDesc.dropperDesc.empty()) {
	//		jsonData.dropGimmickDesc.emplace_back(dropGimmickDesc);
	//	}
	//}

	//stageData_.emplace_back(jsonData);


	static std::string directoryPath = "Resources/Data/StageParts/";

	// パターンに一致するファイルを見つける正規表現パターン
	std::regex pattern("stageParts_([0-9]+)");

	std::map<uint32_t, Desc> stageData{};

	std::vector<Switch::Desc> switchDesc{};
	std::vector<Dropper::Desc> dropperDesc{};

	// ディレクトリ内のファイルを検索し、パターンに一致するファイルを読み込む
	for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
		if (std::filesystem::is_regular_file(entry.path())) {
			std::string fileName = entry.path().stem().string();
			std::smatch match;
			if (std::regex_match(fileName, match, pattern)) {
				Desc jsonData{};
				std::ifstream ifs(entry.path());
				if (!ifs.is_open()) {
					return;
				}
				// JSONをパースしてルートオブジェクトを取得
				nlohmann::json root;
				ifs >> root;
				ifs.close();

				// "objects"配列からオブジェクトを処理
				for (const auto& obj : root["objects"]) {
					// Block
					if (obj.contains("gimmick")) {
						if (obj["gimmick"]["type"] == "Block") {
							Block::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							jsonData.blockDesc.emplace_back(desc);
						}
						else if (obj["gimmick"]["type"] == "Trigger") {
							BossAttackTrigger::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							const auto& gimmick = obj["gimmick"];
							desc.state = static_cast<BossStateManager::State>(gimmick["state"] + 1);
							jsonData.bossAttackTrigger.emplace_back(desc);

						}// Trigger
						else if (obj["gimmick"]["type"] == "Trigger") {
							BossAttackTrigger::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							const auto& gimmick = obj["gimmick"];
							desc.state = static_cast<BossStateManager::State>(gimmick["state"] + 1);
							jsonData.bossAttackTrigger.emplace_back(desc);

						}
						// FireBar
						else if (obj["gimmick"]["type"] == "FireBar") {
							FireBar::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							const auto& gimmick = obj["gimmick"];
							desc.barDesc.length = gimmick["length"];
							desc.barDesc.barInitialAngle = gimmick["initializeAngle"] * Math::ToRadian;
							desc.barDesc.rotateVelocity = gimmick["angularVelocity"] * Math::ToRadian;
							jsonData.fireBarDesc.emplace_back(desc);
						}
						// Floor
						else if (obj["gimmick"]["type"] == "Floor") {
							Floor::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							jsonData.floorDesc.emplace_back(desc);
						}
						// Pendulam
						else if (obj["gimmick"]["type"] == "Pendulum") {
							Pendulum::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							const auto& gimmick = obj["gimmick"];
							desc.length = gimmick["length"];
							desc.angle = gimmick["angle"] * Math::ToRadian;
							desc.initializeAngle = gimmick["initializeAngle"] * Math::ToRadian;
							desc.gravity = gimmick["gravity"];
							desc.stickScale = gimmick["stickScale"];
							desc.ballScale = gimmick["ballScale"];
							jsonData.pendulumDesc.emplace_back(desc);
						}
						// RevengeCoin
						else if (obj["gimmick"]["type"] == "RevengeCoin") {
							RevengeCoin::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							jsonData.revengeCoinDesc.emplace_back(desc);
						}
						// BeltConveyor
						else if (obj["gimmick"]["type"] == "BeltConveyor") {
							BeltConveyor::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							const auto& gimmick = obj["gimmick"];
							desc.velocity = gimmick["beltConveyorVelocity"];
							jsonData.beltConveyorDesc.emplace_back(desc);
						}
						// DroppGimmick
						else if (obj["gimmick"]["type"] == "DropGimmickSwitch") {
							Switch::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							const auto& gimmick = obj["gimmick"];
							desc.index = gimmick["dropGimmickIndex"];
							switchDesc.emplace_back(desc);
						}
						else if (obj["gimmick"]["type"] == "DropGimmickDropper") {
							Dropper::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							const auto& gimmick = obj["gimmick"];
							desc.index = gimmick["dropGimmickIndex"];
							dropperDesc.emplace_back(desc);
						}
					}
					// StageArea
					else if (obj["file_name"] == "stageArea") {
						if (obj.contains("collider")) {
							const auto& collider = obj["collider"];
							jsonData.stageSize = { collider["size"][2] };
						}

					}
					// StageObject
					else if (!obj.contains("gimmick")) {
						StageObject::Desc desc{};
						desc.desc = StageGimmick::GetDesc(obj);
						jsonData.stageObjectDesc.emplace_back(desc);
					}


				}
				// DropGimmickのソート
				// １ステージにおけるスイッチのタイプの数
				for (uint32_t i = 0; i < 100; i++) {
					DropGimmick::Desc dropGimmickDesc{};
					for (auto& desc : switchDesc) {
						if (desc.index == i) {
							dropGimmickDesc.switchDesc.emplace_back(desc);
						}
					}
					for (auto& desc : dropperDesc) {
						if (desc.index == i) {
							dropGimmickDesc.dropperDesc.emplace_back(desc);
						}
					}
					if (!dropGimmickDesc.switchDesc.empty() &&
						!dropGimmickDesc.dropperDesc.empty()) {
						jsonData.dropGimmickDesc.emplace_back(dropGimmickDesc);
					}
				}

				// 正規表現にマッチした部分を数値に変換してステージ番号として使う
				uint32_t index = std::stoi(match[1].str());
				stageData[index] = jsonData;
			}
		}
	}

	// キーを番号でソートするための一時的なベクターを作成
	std::vector<uint32_t> sortedKeys;
	for (const auto& pair : stageData) {
		sortedKeys.push_back(pair.first);
	}

	// 番号でソート
	std::sort(sortedKeys.begin(), sortedKeys.end());

	// ソートされたキーの順序に従ってstageData_を再構築
	for (const auto& key : sortedKeys) {
		stageData_.emplace_back(stageData[key]);
	}

}

void StageLoop::InitializeCreateStage(uint32_t stageIndex) {
	Clear();

	std::vector<uint32_t> stageIndices{};
	float distance = 0.0f;
	for (uint32_t i = 0; i < kCreateStageNum; i++) {
		if (stageIndex == (uint32_t)-1) {
			stageIndex = rnd_.NextUIntRange(0, uint32_t(stageData_.size() - 1));
		}
		if (stageIndices.empty()) {
			// ぎりぎりすぎないよう
			distance = player_->GetWorldMatrix().GetTranslate().z + (stageData_.at(stageIndex).stageSize * 0.5f) - 10.0f;
		}
		else {
			distance += stageData_.at(stageIndices.at(i - 1)).stageSize;
		}
		CreateStageObject(stageData_.at(stageIndex), distance);
		stageIndices.emplace_back(stageIndex);
	}
}

void StageLoop::Clear() {
	bossAttackTriggerManager_->Clear();
	beltConveyorManager_->Clear();
	blockManager_->Clear();
	fireBarManager_->Clear();
	revengeCoinManager_->Clear();
	floorManager_->Clear();
	dropGimmickManager_->Clear();
	pendulumManager_->Clear();
	stageObjectManager_->Clear();
	//trapManager_->Clear();
}

void StageLoop::CreateStage(uint32_t stageIndex) {
	Clear();

	std::vector<uint32_t> stageIndices{};
	float distance = 0.0f;
	for (uint32_t i = 0; i < kCreateStageNum; i++) {

		stageIndex = rnd_.NextUIntRange(0, uint32_t(stageData_.size() - 1));

		if (stageIndices.empty()) {
			// ぎりぎりすぎないよう
			distance = player_->GetWorldMatrix().GetTranslate().z - stageData_.at(stageIndex).stageSize * 0.5f + 10.0f;
		}
		else {
			distance -= stageData_.at(stageIndices.at(i - 1)).stageSize;
		}
		CreateStageObject(stageData_.at(stageIndex), distance);
		stageIndices.emplace_back(stageIndex);
	}
}

void StageLoop::CreateStageObject(const Desc& stageData, float distance) {
	for (const auto& desc : stageData.blockDesc) {
		Block::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		blockManager_->Create(mutableDesc);
	}
	for (const auto& desc : stageData.beltConveyorDesc) {
		BeltConveyor::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		beltConveyorManager_->Create(mutableDesc);
	}
	for (auto& desc : stageData.bossAttackTrigger) {
		BossAttackTrigger::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		bossAttackTriggerManager_->Create(mutableDesc);
	}
	for (auto& desc : stageData.fireBarDesc) {
		FireBar::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		fireBarManager_->Create(mutableDesc);
	}
	for (auto& desc : stageData.floorDesc) {
		Floor::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		floorManager_->Create(mutableDesc);
	}
	for (auto& desc : stageData.dropGimmickDesc) {
		DropGimmick::Desc mutableDesc = desc;
		for (auto& switchDesc : mutableDesc.switchDesc) {
			switchDesc.desc.transform.translate.z += distance;
		}
		for (auto& dropperDesc : mutableDesc.dropperDesc) {
			dropperDesc.desc.transform.translate.z += distance;
		}
		dropGimmickManager_->Create(mutableDesc);
	}
	for (auto& desc : stageData.pendulumDesc) {
		Pendulum::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		pendulumManager_->Create(mutableDesc);
	}
	for (auto& desc : stageData.revengeCoinDesc) {
		RevengeCoin::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		revengeCoinManager_->Create(mutableDesc);
	}
	for (auto& desc : stageData.stageObjectDesc) {
		StageObject::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		stageObjectManager_->Create(mutableDesc);
	}
}
