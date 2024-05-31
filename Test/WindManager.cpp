#include "WindManager.h"

WindManager* WindManager::GetInstance() {
    static WindManager instance;
    return &instance;
}

void WindManager::Initialize() {
    winds_.clear();
}

void WindManager::Update() {
    for (auto it = winds_.begin(); it != winds_.end();) {
        (*it)->Update();
        if (!(*it)->GetIsAlive()) {
            it = winds_.erase(it);
        }
        else {
            ++it;
        }
    }
}

void WindManager::Create(const Wind::Desc& desc) {
	Wind* wind = new Wind();
	wind->Initialize(desc);
	winds_.emplace_back(wind);
}
