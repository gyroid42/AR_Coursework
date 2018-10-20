#pragma once

#ifndef _CAMERA_FEED_BEHAVIOUR_H
#define _CAMERA_FEED_BEHAVIOUR_H


#include "Behaviour.h"

class CameraFeed;
class Input;


class CameraFeedBehaviour : public Behaviour
{
public:
	CameraFeedBehaviour();
	~CameraFeedBehaviour();

	bool Init(GameObject* newGameObject);
	bool CleanUp();
	bool Start();

	bool EarlyUpdate(float dt);

	bool PreRenderGUI(gef::SpriteRenderer* sprite_renderer);
	bool OnGUI(gef::SpriteRenderer* sprite_renderer);

	bool OnDestroy();

private:
	
	CameraFeed* camera_feed_;
	Input* input_;
};

#endif