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

void BossBulletManager::Create(const Transform& parent,const Vector3& pos, const Vector3& velocity) {
	BossBullet* bossBullet = new BossBullet();
	bossBullet->Initialize(parent,pos, velocity);
	bullets_.emplace_back(std::move(bossBullet));
}

void BossBulletManager::SetVelocity(uint32_t index,float velocity) {
    if (index >= bullets_.size()) {
        throw std::out_of_range("Index out of range");
    }

    auto it = bullets_.begin();
    std::advance(it, index);
    (*it)->SetVelocity(velocity);
}

void BossBulletManager::Reset() {
	bullets_.clear();
}
