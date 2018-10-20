

#include "LevelManager.h"
#include "Level.h"
#include "TestLevel.h"


LevelManager::LevelManager(gef::Platform& platform) :
	platform_(platform),
	current_level_(nullptr),
	next_level_(nullptr)
{

}

LevelManager::~LevelManager() {

}



LevelManager* LevelManager::level_manager_ = nullptr;


bool LevelManager::Init() {


	if (!level_manager_) {

		level_manager_ = this;
	}
	else if (level_manager_ != this) {

		return false;
	}


	InitLevelList();


	return true;
}


bool LevelManager::CleanUp() {


	UnloadCurrentLevel();
	CleanUpLevels();

	for (auto& iterator : levels_) {

		delete iterator.second;
		iterator.second = nullptr;
	}

	levels_.clear();

	return true;
}


bool LevelManager::InitLevelList() {

	TestLevel* new_test_level = new TestLevel(platform_);
	new_test_level->Init();

	levels_.insert(std::make_pair(TEST_LEVEL, new_test_level));

	return true;
}


bool LevelManager::Update() {

	if (changing_level_) {

		LoadNextLevel();
		changing_level_ = false;
	}

	return true;
}




bool LevelManager::LoadNextLevel() {

	UnloadCurrentLevel();

	if (!next_level_->Load()) {

		return false;
	}

	current_level_ = next_level_;


	return true;
}

bool LevelManager::UnloadCurrentLevel() {

	if (current_level_) {

		current_level_->Unload();
	}

	return true;
}

bool LevelManager::CleanUpLevels() {


	for (auto& iterator : levels_) {

		iterator.second->CleanUp();
	}

	

	return true;
}


bool LevelManager::ChangeLevel(LEVELS new_level) {


	auto iterator = levels_.find(new_level);

	if (iterator == levels_.end()) {

		return false;
	}

	next_level_ = iterator->second;
	changing_level_ = true;


	return true;
}



