#pragma once

#ifndef _LEVEL_H
#define _LEVEL_H


#include <system\Platform.h>



class Level
{
public:
	Level(gef::Platform& platform);
	virtual ~Level();

	virtual bool Init();
	virtual bool CleanUp();

	virtual bool Load();
	virtual bool Unload();

protected:

	gef::Platform& platform_;
};

#endif
