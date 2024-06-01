#pragma once

#include <memory>
#include <list>

#include "Wind.h"

class WindManager {
public:
	static WindManager* GetInstance();
	WindManager(const WindManager&) = delete;
	WindManager& operator=(const WindManager&) = delete;
	void Initialize();

	void Update();

	void Create(const Wind::Desc& desc);

private:
	WindManager() = default;
	~WindManager() = default;
	std::list<std::unique_ptr<Wind>> winds_;
};