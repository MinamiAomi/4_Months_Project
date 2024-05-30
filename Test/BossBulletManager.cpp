#include "BossBulletManager.h"

BossBulletManager* BossBulletManager::GetInstance() {
    static BossBulletManager instance;
    return &instance;
}

void BossBulletManager::Initialize() {
    Reset();
}

void BossBulletManager::Update() {
    for (auto it = bullets_.begin(); it != bullets_.end();) {
        (*it)->Update();
        if (!(*it)->GetIsAlive()) {
            it = bullets_.erase(it); 
        }
        else {
            ++it; 
        }
    }
}

void BossBulletManager::Create(const Vector3& pos, const Vector3& velocity) {
	BossBullet* bossBullet = new BossBullet();
	bossBullet->Initialize(pos, velocity);
	bullets_.emplace_back(std::move(bossBullet));
}

void BossBulletManager::Reset() {
	bullets_.clear();
}
