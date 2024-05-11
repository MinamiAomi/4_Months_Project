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
	fireBarManager_ = std::make_unique<FireBarManager>();
	floorManager_ = std::make_unique<FloorManager>();
	pendulumManager_ = std::make_unique<PendulumManager>();
	revengeCoinManager_ = std::make_unique<RevengeCoinManager>();
	stageObjectManager_ = std::make_unique<StageObjectManager>();
	trapManager_ = std::make_unique<TrapManager>();

	bossAttackTriggerManager_->SetCamera(camera_);
	bossAttackTriggerManager_->SetBoss(boss_);
	blockManager_->SetCamera(camera_);
	blockManager_->SetPlayer(player_);
	fireBarManager_->SetCamera(camera_);
	fireBarManager_->SetPlayer(player_);
	floorManager_->SetCamera(camera_);
	floorManager_->SetPlayer(player_);
	pendulumManager_->SetCamera(camera_);
	pendulumManager_->SetPlayer(player_);
	revengeCoinManager_->SetCamera(camera_);
	revengeCoinManager_->SetPlayer(player_);
	stageObjectManager_->SetCamera(camera_);
	stageObjectManager_->SetPlayer(player_);
	trapManager_->SetCamera(camera_);
	trapManager_->SetPlayer(player_);
	trapManager_->Initialize();

	LoadJson();

	CreateStage();

	isCreateStage_ = false;
}

void StageLoop::Update() {
	if (Character::currentCharacterState_ == Character::State::kScneChange&&
		Character::nextCharacterState_ == Character::State::kRunAway&&
		!isCreateStage_) {
		isCreateStage_ = true;
		CreateStage();
	}
	else if(Character::currentCharacterState_ == Character::State::kRunAway){
		isCreateStage_ = false;
	}

	blockManager_->Update();
	fireBarManager_->Update();
	floorManager_->Update();
	revengeCoinManager_->Update();
	pendulumManager_->Update();
	stageObjectManager_->Update();
	trapManager_->Update();
	bossAttackTriggerManager_->Update();
}

void StageLoop::Reset() {
	CreateStage();
}

void StageLoop::LoadJson() {
	static std::string directoryPath = "Resources/Data/StageParts/";

	// パターンに一致するファイルを見つける正規表現パターン
	std::regex pattern("stageParts_([0-9]+)");

	std::map<uint32_t, Desc> stageData{};

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
					if (obj.contains("gimmick") &&
						obj["gimmick"]["type"] == "Block") {
						Block::Desc desc{};
						desc.desc = StageGimmick::GetDesc(obj);
						jsonData.blockDesc.emplace_back(desc);
					}
					// Trigger
					else if (obj.contains("gimmick") &&
						obj["gimmick"]["type"] == "Trigger") {
						BossAttackTrigger::Desc desc{};
						desc.desc = StageGimmick::GetDesc(obj);
						const auto& gimmick = obj["gimmick"];
						desc.state = static_cast<BossStateManager::State>(gimmick["state"] + 1);
						jsonData.bossAttackTrigger.emplace_back(desc);

					}
					// FireBar
					else if (obj.contains("gimmick") &&
						obj["gimmick"]["type"] == "FireBar") {
						FireBar::Desc desc{};
						desc.desc = StageGimmick::GetDesc(obj);
						const auto& gimmick = obj["gimmick"];
						desc.barDesc.length = gimmick["length"];
						desc.barDesc.barInitialAngle = gimmick["initializeAngle"] * Math::ToRadian;
						desc.barDesc.rotateVelocity = gimmick["angularVelocity"] * Math::ToRadian;
						jsonData.fireBarDesc.emplace_back(desc);
					}
					// Floor
					else if (obj.contains("gimmick") &&
						obj["gimmick"]["type"] == "Floor") {
						Floor::Desc desc{};
						desc.desc = StageGimmick::GetDesc(obj);
						jsonData.floorDesc.emplace_back(desc);
					}
					// Pendulam
					else if (obj.contains("gimmick") &&
						obj["gimmick"]["type"] == "Pendulum") {
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
					else if (obj.contains("gimmick") &&
						obj["gimmick"]["type"] == "RevengeCoin") {
						RevengeCoin::Desc desc{};
						desc.desc = StageGimmick::GetDesc(obj);
						jsonData.revengeCoinDesc.emplace_back(desc);
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

void StageLoop::Clear() {
	bossAttackTriggerManager_->Clear();
	blockManager_->Clear();
	fireBarManager_->Clear();
	revengeCoinManager_->Clear();
	floorManager_->Clear();
	pendulumManager_->Clear();
	stageObjectManager_->Clear();
	trapManager_->Clear();
}

void StageLoop::CreateStage() {
	Clear();
	static uint32_t kCreateStageNum = 5;
	std::vector<uint32_t> stageIndices{};
	float distance = 0.0f;
	for (uint32_t i = 0; i < kCreateStageNum; i++) {
		uint32_t stageIndex = rnd_.NextUIntRange(0, uint32_t(stageData_.size()-1));
		if (stageIndices.empty()) {
			// ぎりぎりすぎないよう
			distance = player_->GetWorldMatrix().GetTranslate().z- stageData_.at(stageIndex).stageSize * 0.5f+10.0f;
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
