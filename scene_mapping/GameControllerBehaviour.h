#pragma once

#ifndef _GAME_CONTROLLER_BEHAVIOUR_H
#define _GAME_CONTROLLER_BEHAVIOUR_H

#include "Behaviour.h"
#include "Text.h"

#include <sony_tracking.h>

class GameObjectManager;
class CameraFeed;

class GameControllerBehaviour :
	public Behaviour
{
public:
	GameControllerBehaviour();
	~GameControllerBehaviour();

	bool Init(GameObject* newGameObject);

	bool Start();

	bool Update(float dt);
	bool LateUpdate(float dt);

	bool OnGUI(gef::SpriteRenderer* sprite_renderer);

	bool OnDestroy();





private:

	GameObjectManager* gameobject_manager_;
	CameraFeed* camera_feed_;


	SceSmartSceneMappingState scene_mapping_state_;
	SceSmartSceneMappingInitMode scene_mapping_mode_;
	float fps_;

	Text fps_text_;
	Text scene_map_mode_text_; 
	Text scene_map_state_text_;

};

#endif