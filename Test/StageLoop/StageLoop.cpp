#include "StageLoop.h"

#include <filesystem>
#include <fstream>
#include <regex>

#include "Boss/Boss.h"
#include "CameraManager/CameraManager.h"
#include "Player/Player.h"
#include "CharacterState.h"
#include "File/JsonConverter.h"

#include "HammerMovie.h"
#include "Engine/Graphics/ImGuiManager.h"
#include "File/JsonHelper.h"
#include "Externals/nlohmann/json.hpp"

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
	dropGimmickManager_->Initialize();
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

	stageNum_ = 0;
	playerCurrentStageNum_ = 0;
	InitializeCreateStage(0);

	isCreateStage_ = false;
}

void StageLoop::Update() {

#ifdef _DEBUG
	Debug();
#endif // _DEBUG
	if (hammerMovie_->IsHitFrame() && !isCreateStage_) {
		isCreateStage_ = true;
		CreateStage();
	}
	else if (!boss_->GetIsFirstHit()) {
		TutorialCreateStage();

	}
	else if (Character::currentCharacterState_ == Character::State::kRunAway) {
		isCreateStage_ = false;
		AddStage();
	}

	blockManager_->Update();
	fireBarManager_->Update();
	floorManager_->Update();
	dropGimmickManager_->Update();
	revengeCoinManager_->Update();
	pendulumManager_->Update();
	stageObjectManager_->Update();
	// trapManager_->Update();
	bossAttackTriggerManager_->Update();
	beltConveyorManager_->Update();
}


void StageLoop::Reset() {
	stageNum_ = 0;
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
	//			if (gimmick.contains("beamVector")) {
	//				desc.vector = gimmick["beamVector"].get<Vector3>();;
	//			}
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
	//		else if (obj["gimmick"]["type"] == "DropGimmickBall") {
	//			DropperBall::Desc desc{};
	//			desc.desc = StageGimmick::GetDesc(obj);
	//			jsonData.dropGimmickBallDesc.emplace_back(desc);
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

				std::vector<Switch::Desc> switchDesc{};
				std::vector<Dropper::Desc> dropperDesc{};

				// "objects"配列からオブジェクトを処理
				for (const auto& obj : root["objects"]) {
					// Block
					if (obj.contains("gimmick")) {
						if (obj["gimmick"]["type"] == "Block") {
							Block::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							jsonData.blockDesc.emplace_back(desc);
						}
						// Trigger
						else if (obj["gimmick"]["type"] == "Trigger") {
							BossAttackTrigger::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							const auto& gimmick = obj["gimmick"];
							desc.state = static_cast<BossStateManager::State>(gimmick["state"] + 1);
							if (gimmick.contains("beamVector")) {
								desc.vector = gimmick["beamVector"].get<Vector3>();;
							}
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
						else if (obj["gimmick"]["type"] == "DropGimmickBall") {
							DropperBall::Desc desc{};
							desc.desc = StageGimmick::GetDesc(obj);
							jsonData.dropGimmickBallDesc.emplace_back(desc);
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
				std::unordered_map<uint32_t, DropGimmick::Desc> groupedDesc;

				// switchDescをindexでグループ化
				for (const auto& desc : switchDesc) {
					groupedDesc[desc.index].switchDesc.emplace_back(desc);
				}

				// dropperDescをindexでグループ化
				for (const auto& desc : dropperDesc) {
					groupedDesc[desc.index].dropperDesc.emplace_back(desc);
				}

				// groupedDescからjsonData.dropGimmickDescを作成
				for (auto& [index, dropGimmickDesc] : groupedDesc) {
					if (!dropGimmickDesc.switchDesc.empty() && !dropGimmickDesc.dropperDesc.empty()) {
						jsonData.dropGimmickDesc.emplace_back(std::move(dropGimmickDesc));
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
	// ステージ確率のやつ
	{
		// ファイルを開く
		std::ifstream inputFile("Resources/Data/StageLoop/StageLoop.json");

		// ファイル内容を文字列として読み込む
		std::string fileContent((std::istreambuf_iterator<char>(inputFile)),
			std::istreambuf_iterator<char>());

		// JSON文字列を解析
		auto jsonParsed = nlohmann::json::parse(fileContent);

		// levelDesc_配列をクリア
		levelDesc_.clear();

		// JSONオブジェクトをループ
		for (auto& [key, value] : jsonParsed.items()) {
			LevelDesc levelDesc;

			// 各StageLoopのデータを解析
			levelDesc.stage.min = value["levelDesc.stage.min"];
			levelDesc.stage.max = value["levelDesc.stage.max"];

			// Probabilityのデータを解析
			int index = 0;
			while (value.contains("Probability" + std::to_string(index))) {
				levelDesc.probability.push_back(value["Probability" + std::to_string(index)]["probability"]);
				index++;
			}

			// 配列に追加
			levelDesc_.push_back(levelDesc);
		}
		levelDivision_ = int(levelDesc_.size());
		bossHPDivision_ = int(levelDesc_.at(0).probability.size());
	}
}

void StageLoop::InitializeCreateStage(uint32_t stageInputIndex) {
	Clear();

	float distance = 0.0f;
	uint32_t stageIndex;
	stageDistance_.clear();
	for (uint32_t i = 0; i < kCreateStageNum; i++) {
		StageDistance stageDistance{};
		// 指定がなければランダム
		if (stageInputIndex == (uint32_t)-1) {
			stageIndex = rnd_.NextUIntRange(1, uint32_t(stageData_.size()) - 1);
		}
		else {
			stageIndex = stageInputIndex;
		}
		if (stageDistance_.empty()) {
			// ぎりぎりすぎないよう
			distance = player_->GetWorldMatrix().GetTranslate().z + (stageData_.at(stageIndex).stageSize * 0.5f) - 10.0f;
		}
		else {
			distance += stageData_.at(stageDistance_.at(i - 1).stageIndex).stageSize * 0.5f + stageData_.at(stageIndex).stageSize * 0.5f;
		}
		CreateStageObject(stageData_.at(stageIndex), distance, stageNum_);
		stageDistance.distance = distance;
		stageDistance.stageIndex = stageIndex;
		stageDistance.stageNum = stageNum_;
		stageDistance_.emplace_back(stageDistance);
		stageNum_++;
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
}

void StageLoop::DeleteObject(uint32_t index) {

	uint32_t leaveIndex = index;

	auto& bossTriggers = bossAttackTriggerManager_->GetBossAttackTriggers();
	for (auto it = bossTriggers.begin(); it != bossTriggers.end(); ) {
		if ((*it)->stageGimmickNumber == leaveIndex) {
			it = bossTriggers.erase(it);
		}
		else {
			++it;
		}
	}

	auto& beltConveyorManager = beltConveyorManager_->GetBlocks();
	for (auto it = beltConveyorManager.begin(); it != beltConveyorManager.end(); ) {
		if ((*it)->stageGimmickNumber == leaveIndex) {
			it = beltConveyorManager.erase(it);
		}
		else {
			// 削除しない場合は通常通りインクリメントする
			++it;
		}
	}

	auto& blockManager = blockManager_->GetBlocks();
	for (auto it = blockManager.begin(); it != blockManager.end(); ) {
		if ((*it)->stageGimmickNumber == leaveIndex) {
			it = blockManager.erase(it);
		}
		else {
			// 削除しない場合は通常通りインクリメントする
			++it;
		}
	}

	auto& fireBarManager = fireBarManager_->GetFireBars();
	for (auto it = fireBarManager.begin(); it != fireBarManager.end(); ) {
		if ((*it)->stageGimmickNumber == leaveIndex) {
			it = fireBarManager.erase(it);
		}
		else {
			// 削除しない場合は通常通りインクリメントする
			++it;
		}
	}


	auto& revengeCoinManager = revengeCoinManager_->GetBlocks();
	for (auto it = revengeCoinManager.begin(); it != revengeCoinManager.end(); ) {
		if ((*it)->stageGimmickNumber == leaveIndex) {
			it = revengeCoinManager.erase(it);
		}
		else {
			// 削除しない場合は通常通りインクリメントする
			++it;
		}
	}


	auto& floorManager = floorManager_->GetFloors();
	for (auto it = floorManager_->GetFloors().begin(); it != floorManager_->GetFloors().end(); ) {
		if ((*it)->stageGimmickNumber == leaveIndex) {
			it = floorManager.erase(it);
		}
		else {
			// 削除しない場合は通常通りインクリメントする
			++it;
		}
	}


	auto& dropGimmickManager = dropGimmickManager_->GetDropGimmicks();
	for (auto it = dropGimmickManager.begin(); it != dropGimmickManager.end(); ) {
		if ((*it)->stageGimmickNumber == leaveIndex) {
			it = dropGimmickManager.erase(it);
		}
		else {
			// 削除しない場合は通常通りインクリメントする
			++it;
		}
	}

	auto& dropperBallManager = dropGimmickManager_->GetDropperBallManager()->GetDropGimmicks();
	for (auto it = dropperBallManager.begin(); it != dropperBallManager.end(); ) {
		if ((*it)->stageGimmickNumber == leaveIndex) {
			it = dropperBallManager.erase(it);;
		}
		else {
			// 削除しない場合は通常通りインクリメントする
			++it;
		}
	}


	auto& pendulumManager = pendulumManager_->GetPendulums();
	for (auto it = pendulumManager.begin(); it != pendulumManager.end(); ) {
		if ((*it)->stageGimmickNumber == leaveIndex) {
			it = pendulumManager.erase(it);
		}
		else {
			// 削除しない場合は通常通りインクリメントする
			++it;
		}
	}


	auto& stageObjectManager = stageObjectManager_->GetStageObjects();
	for (auto it = stageObjectManager.begin(); it != stageObjectManager.end(); ) {
		if ((*it)->stageGimmickNumber == leaveIndex) {
			// イテレータを削除後にインクリメントする
			it = stageObjectManager.erase(it);
		}
		else {
			// 削除しない場合は通常通りインクリメントする
			++it;
		}
	}
}

void StageLoop::CheckOnPlayerStageParts() {
	bool found = false;
	uint32_t index = 0;
	uint32_t leaveStageNum = 0;
	uint32_t leaveNextStageNum = 0;

	// プレイヤーの位置に基づいて leaveIndex と leaveNextIndex を決定
	for (uint32_t i = 0; i < stageDistance_.size(); ++i) {
		const auto& stageDistance = stageDistance_[i];
		float playerZ = player_->transform.worldMatrix.GetTranslate().z;
		float stageMinZ = stageDistance.distance - stageData_.at(stageDistance.stageIndex).stageSize * 0.5f;
		float stageMaxZ = stageDistance.distance + stageData_.at(stageDistance.stageIndex).stageSize * 0.5f;

		if (playerZ >= stageMinZ && playerZ <= stageMaxZ) {
			index = i;
			leaveStageNum = stageDistance.stageNum;
			found = true;
			playerCurrentStageNum_ = leaveStageNum;
			break;
		}
	}

	if (stageDistance_.at(index).distance > 0) {
		leaveNextStageNum = stageDistance_.at(index + 1).stageNum;
	}
	else {
		leaveNextStageNum = stageDistance_.at(index - 1).stageNum;
	}
	// leaveIndex と leaveNextIndex を残す
	stageDistance_.erase(
		std::remove_if(stageDistance_.begin(), stageDistance_.end(),
			[leaveStageNum, leaveNextStageNum](const StageDistance& stageDistance) {
				return stageDistance.stageNum != leaveStageNum && stageDistance.stageNum != leaveNextStageNum;
			}),
		stageDistance_.end());

	// distance を大きい順にソート
	std::sort(stageDistance_.begin(), stageDistance_.end(),
		[](const StageDistance& a, const StageDistance& b) {
			return a.distance > b.distance;
		});
}

void StageLoop::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("StageLoop")) {
		ImGui::DragInt("ボスのHPを何分割するか", &bossHPDivision_, 1, 0);
		for (auto& levelDesc : levelDesc_) {
			if (levelDesc.probability.size() != bossHPDivision_) {
				levelDesc.probability.resize(bossHPDivision_);
			}
		}

		ImGui::DragInt("レベルを何分割するか", &levelDivision_, 1, 0);
		if (levelDesc_.size() != levelDivision_) {
			// 同じサイズに調整する
			levelDesc_.resize(levelDivision_);
		}
		if (ImGui::TreeNode("ステージの幅")) {
			for (int level = 0; level < levelDivision_; ++level) {
				if (ImGui::TreeNode((std::to_string(level) + "レベル").c_str())) {
					ImGui::DragInt((std::to_string(level) + "最小").c_str(), &levelDesc_.at(level).stage.min, 1, 0);
					ImGui::DragInt((std::to_string(level) + "最大").c_str(), &levelDesc_.at(level).stage.max, 1, levelDesc_.at(level).stage.min);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		for (int gauge = 0; gauge < bossHPDivision_; ++gauge) {
			if (ImGui::TreeNode((std::to_string(gauge) + "ゲージ目").c_str())) {
				for (int level = 0; level < levelDivision_; ++level) {
					ImGui::DragFloat((std::to_string(level) + "レベルの確率").c_str(), &levelDesc_.at(level).probability.at(gauge), 1.0f, 0.0f);
				}
				ImGui::TreePop();
			}
		}
		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/StageLoop/StageLoop.json");
			for (uint32_t i = 0; auto & levelDesc : levelDesc_) {
				JSON_OBJECT("StageLoop" + std::to_string(i));
				JSON_SAVE(levelDesc.stage.min);
				JSON_SAVE(levelDesc.stage.max);
				JSON_ROOT();
				for (uint32_t j = 0; auto & probability : levelDesc.probability) {
					JSON_OBJECT("StageLoop" + std::to_string(i));
					JSON_OBJECT("Probability" + std::to_string(j));
					JSON_SAVE(probability);
					JSON_ROOT();
					j++;
				}
				JSON_ROOT();
				++i;
			}
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}

void StageLoop::CreateStage(uint32_t stageInputIndex) {
	CheckOnPlayerStageParts();

	Clear();

	// 中身確認
	//DeleteObject();

	// プレイヤーが現在いるパーツとその前のパーツの処理
	float distance = stageDistance_.at(0).distance;
	CreateStageObject(stageData_.at(stageDistance_.at(0).stageIndex), distance, stageNum_);
	stageNum_++;
	distance -= stageData_.at(stageDistance_.at(0).stageIndex).stageSize * 0.5f + stageData_.at(stageDistance_.at(1).stageIndex).stageSize * 0.5f;
	CreateStageObject(stageData_.at(stageDistance_.at(1).stageIndex), distance, stageNum_);
	stageNum_++;

	uint32_t stageIndex = 0;
	// BossHPが今何割か
	auto hp = boss_->GetBossHP()->GetCurrentHP();
	int32_t maxHP = boss_->GetBossHP()->kMaxHP;
	float hpRatio = 1.0f - (static_cast<float>(hp) / static_cast<float>(maxHP));
	uint32_t levelCount = static_cast<uint32_t>(levelDesc_.size()) - 1;

	// ここの部分でどのlevelDescを使うか
	uint32_t currentLevel = std::clamp(static_cast<uint32_t>(std::lerp(0.0f, static_cast<float>(levelCount), hpRatio)), 0u, levelCount);
	for (uint32_t i = 2; i < kCreateStageNum + 2; i++) {
		StageDistance stageDistance{};
		if (stageInputIndex == (uint32_t)-1) {

			// 確率をとりどこのレベルを使用するか
			float probability = std::lerp(0.0f, 100.0f, rnd_.NextFloatUnit());
			float sum = 0.0f;

			for (size_t j = 0; j < levelDesc_.at(currentLevel).probability.size(); ++j) {
				sum += levelDesc_.at(currentLevel).probability.at(j);
				if (sum >= probability) {
					stageIndex = rnd_.NextUIntRange(levelDesc_.at(currentLevel).stage.min, levelDesc_.at(currentLevel).stage.max);
					break;
				}
			}
		}
		else {
			stageIndex = stageInputIndex;
		}

		distance -= (stageData_.at(stageDistance_.at(i - 1).stageIndex).stageSize * 0.5f) + (stageData_.at(stageIndex).stageSize * 0.5f);


		CreateStageObject(stageData_.at(stageIndex), distance, stageNum_);
		stageDistance.distance = distance;
		stageDistance.stageIndex = stageIndex;
		stageDistance.stageNum = stageNum_;
		stageDistance_.emplace_back(stageDistance);
		stageNum_++;
	}
}

void StageLoop::CreateStageObject(const Desc& stageData, float distance, uint32_t index) {
	for (const auto& desc : stageData.blockDesc) {
		Block::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		blockManager_->Create(mutableDesc, index);
	}
	for (const auto& desc : stageData.beltConveyorDesc) {
		BeltConveyor::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		beltConveyorManager_->Create(mutableDesc, index);
	}
	for (auto& desc : stageData.bossAttackTrigger) {
		BossAttackTrigger::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		bossAttackTriggerManager_->Create(mutableDesc, index);
	}
	for (auto& desc : stageData.fireBarDesc) {
		FireBar::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		fireBarManager_->Create(mutableDesc, index);
	}
	for (auto& desc : stageData.floorDesc) {
		Floor::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		floorManager_->Create(mutableDesc, index);
	}
	for (auto& desc : stageData.dropGimmickDesc) {
		DropGimmick::Desc mutableDesc = desc;
		for (auto& switchDesc : mutableDesc.switchDesc) {
			switchDesc.desc.transform.translate.z += distance;
		}
		for (auto& dropperDesc : mutableDesc.dropperDesc) {
			dropperDesc.desc.transform.translate.z += distance;
		}
		dropGimmickManager_->Create(mutableDesc, index);
	}
	for (auto& desc : stageData.dropGimmickBallDesc) {
		DropperBall::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		dropGimmickManager_->Create(mutableDesc, index);
	}
	for (auto& desc : stageData.pendulumDesc) {
		Pendulum::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		pendulumManager_->Create(mutableDesc, index);
	}
	for (auto& desc : stageData.revengeCoinDesc) {
		RevengeCoin::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		revengeCoinManager_->Create(mutableDesc, index);
	}
	for (auto& desc : stageData.stageObjectDesc) {
		StageObject::Desc mutableDesc = desc;
		mutableDesc.desc.transform.translate.z += distance;
		stageObjectManager_->Create(mutableDesc, index);
	}
}

void StageLoop::TutorialCreateStage() {
	bool found = false;
	uint32_t index = 0;
	uint32_t leaveStageNum = 0;

	// プレイヤーの位置に基づいて leaveIndex と leaveNextIndex を決定
	for (uint32_t i = 0; i < stageDistance_.size(); ++i) {
		const auto& stageDistance = stageDistance_[i];
		float playerZ = player_->transform.worldMatrix.GetTranslate().z;
		float stageMinZ = stageDistance.distance - stageData_.at(stageDistance.stageIndex).stageSize * 0.5f;
		float stageMaxZ = stageDistance.distance + stageData_.at(stageDistance.stageIndex).stageSize * 0.5f;

		if (playerZ >= stageMinZ && playerZ <= stageMaxZ) {
			index = i;
			leaveStageNum = stageDistance.stageNum;
			playerCurrentStageNum_ = leaveStageNum;
			found = true;
			break;
		}
	}
	// leaveIndex と leaveNextIndex を残す
	stageDistance_.erase(
		std::remove_if(stageDistance_.begin(), stageDistance_.end(),
			[leaveStageNum](const StageDistance& stageDistance) {
				return stageDistance.stageNum < leaveStageNum;
			}),
		stageDistance_.end());

	if (stageDistance_.size() < kCreateStageNum) {
		if (leaveStageNum > 2) {
			DeleteObject(leaveStageNum - 2);
		}
		float distance = 0.0f;
		uint32_t stageIndex = 0;
		StageDistance stageDistance{};
		distance += stageDistance_.back().distance + stageData_.at(stageIndex).stageSize * 0.5f;

		CreateStageObject(stageData_.at(stageIndex), distance, stageNum_);
		stageDistance.distance = distance;
		stageDistance.stageIndex = stageIndex;
		stageDistance.stageNum = stageNum_;
		stageDistance_.emplace_back(stageDistance);
		stageNum_++;
	}
}

void StageLoop::AddStage() {
	bool found = false;
	uint32_t index = 0;
	uint32_t leaveStageNum = 0;
	uint32_t prePlayerCurrentStageNum = playerCurrentStageNum_;
	// プレイヤーの位置に基づいて leaveIndex と leaveNextIndex を決定
	for (uint32_t i = 0; i < stageDistance_.size(); ++i) {
		const auto& stageDistance = stageDistance_[i];
		float playerZ = player_->transform.worldMatrix.GetTranslate().z;
		float stageMinZ = stageDistance.distance - stageData_.at(stageDistance.stageIndex).stageSize * 0.5f;
		float stageMaxZ = stageDistance.distance + stageData_.at(stageDistance.stageIndex).stageSize * 0.5f;

		if (playerZ >= stageMinZ && playerZ <= stageMaxZ) {
			index = i;
			leaveStageNum = stageDistance.stageNum;
			playerCurrentStageNum_ = leaveStageNum;
			found = true;
			break;
		}
	}



	//// leaveIndex と leaveNextIndex を残す
	//stageDistance_.erase(
	//	std::remove_if(stageDistance_.begin(), stageDistance_.end(),
	//		[leaveStageNum](const StageDistance& stageDistance) {
	//			return stageDistance.stageNum < leaveStageNum;
	//		}),
	//	stageDistance_.end());

	if (playerCurrentStageNum_ > prePlayerCurrentStageNum) {
		uint32_t stageIndex = 0;
		// BossHPが今何割か
		auto hp = boss_->GetBossHP()->GetCurrentHP();
		int32_t maxHP = boss_->GetBossHP()->kMaxHP;
		float hpRatio = 1.0f - (static_cast<float>(hp) / static_cast<float>(maxHP));
		uint32_t levelCount = static_cast<uint32_t>(levelDesc_.size()) - 1;

		// ここの部分でどのlevelDescを使うか
		uint32_t currentLevel = std::clamp(static_cast<uint32_t>(std::lerp(0.0f, static_cast<float>(levelCount), hpRatio)), 0u, levelCount);
		StageDistance stageDistance{};

		// 確率をとりどこのレベルを使用するか
		float probability = std::lerp(0.0f, 100.0f, rnd_.NextFloatUnit());
		float sum = 0.0f;

		for (size_t j = 0; j < levelDesc_.at(currentLevel).probability.size(); ++j) {
			sum += levelDesc_.at(currentLevel).probability.at(j);
			if (sum >= probability) {
				stageIndex = rnd_.NextUIntRange(levelDesc_.at(currentLevel).stage.min, levelDesc_.at(currentLevel).stage.max);
				break;
			}
		}

		float distance = stageDistance_.back().distance - stageData_.at(stageDistance_.back().stageIndex).stageSize * 0.5f - (stageData_.at(stageIndex).stageSize * 0.5f);


		CreateStageObject(stageData_.at(stageIndex), distance, stageNum_);
		stageDistance.distance = distance;
		stageDistance.stageIndex = stageIndex;
		stageDistance.stageNum = stageNum_;
		stageDistance_.emplace_back(stageDistance);
		stageNum_++;
	}
}
