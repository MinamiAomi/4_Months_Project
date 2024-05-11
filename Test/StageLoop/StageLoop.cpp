#include "StageLoop.h"

#include <filesystem>
#include <fstream>
#include <regex>

#include "Boss/Boss.h"
#include "CameraManager/CameraManager.h"
#include "Player/Player.h"

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

	bossAttackTriggerManager_->Initialize(0);
	blockManager_->Initialize(0);
	fireBarManager_->Initialize(0);
	floorManager_->Initialize(0);
	revengeCoinManager_->Initialize(0);
	pendulumManager_->Initialize(0);
	stageObjectManager_->Initialize(0);
	trapManager_->Initialize();

	LoadJson();
}

void StageLoop::Update() {
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
	bossAttackTriggerManager_->Reset(0);
	blockManager_->Reset(0);
	fireBarManager_->Reset(0);
	revengeCoinManager_->Reset(0);
	floorManager_->Reset(0);
	pendulumManager_->Reset(0);
	trapManager_->Reset();
}

void StageLoop::LoadJson() {
	static std::string directoryPath = "Resources/Data/StageScene/";

	// パターンに一致するファイルを見つける正規表現パターン
	std::regex pattern("stageScene([0-9]+)");

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
					// StageObject
					else if (!obj.contains("gimmick")) {
						StageObject::Desc desc{};
						desc.desc = StageGimmick::GetDesc(obj);
						jsonData.stageObjectDesc.emplace_back(desc);
					}
					// StageArea
					else if () {

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

void StageLoop::CreateStage() {
	static uint32_t kCreateStageNum = 4;
	std::vector<uint32_t> stageIndices{};
	for (uint32_t i = 0; i < kCreateStageNum; i++) {
		if (stageIndices.empty()) {

		}
		else {
			uint32_t stageIndex = rnd_.NextUIntRange(0, stageData_.size());
			CreateStageObject(stageData_.at(stageIndex));
			stageIndices.emplace_back(stageIndex);
		}
	}
}

void StageLoop::CreateStageObject(const Desc& stageData,float distance) {
	for (const auto& desc : stageData.blockDesc) { 
		Block::Desc mutableDesc = desc; 
		mutableDesc.desc.transform.translate.z += distance; 
		blockManager_->Create(mutableDesc);
	}
	for (auto& desc : stageData.bossAttackTrigger) {
		BossAttackTrigger::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		bossAttackTriggerManager_->Create(desc);
	}
	for (auto& desc : stageData.fireBarDesc) {
		FireBar::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		fireBarManager_->Create(desc);
	}
	for (auto& desc : stageData.floorDesc) {
		Floor::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		floorManager_->Create(desc);
	}
	for (auto& desc : stageData.pendulumDesc) {
		Pendulum::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		pendulumManager_->Create(desc);
	}
	for (auto& desc : stageData.revengeCoinDesc) {
		RevengeCoin::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		revengeCoinManager_->Create(desc);
	}
	for (auto& desc : stageData.stageObjectDesc) {
		StageObject::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		stageObjectManager_->Create(desc);
	}
}
