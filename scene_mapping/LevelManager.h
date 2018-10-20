#pragma once

#ifndef _LEVEL_MANAGER_H
#define _LEVEL_MANAGER_H


#include <string>
#include <map>
#include <system\Platform.h>

class Level;


enum LEVELS {
	DEFAULT,
	TEST_LEVEL,
	SPLASH_SCREEN
};


class LevelManager 
{
public:
	LevelManager(gef::Platform& platform);
	~LevelManager();

	bool Init();
	bool CleanUp();




	bool Update();


	bool ChangeLevel(LEVELS new_level);

	bool UnloadCurrentLevel();
	bool CleanUpLevels();



	Level* CurrentLevel() { return current_level_; }


	inline static LevelManager* GetLevelManager() { return level_manager_; }

private:

	bool InitLevelList();
	bool LoadNextLevel();


	gef::Platform& platform_;


	static LevelManager* level_manager_;

	Level* current_level_;

	std::map<LEVELS, Level*> levels_;


	Level* next_level_;
	bool changing_level_;


};


#endif