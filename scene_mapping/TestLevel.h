#pragma once

#ifndef _TEST_LEVEL_H
#define _TEST_LEVEL_H

#include "Level.h"

class CameraFeed;


class TestLevel : public Level
{
public:
	TestLevel(gef::Platform& platform);
	~TestLevel();

	bool Init();
	bool CleanUp();

	bool Load();
	bool Unload();
};


#endif