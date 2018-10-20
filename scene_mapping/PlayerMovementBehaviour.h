#pragma once

#ifndef _PLAYER_MOVEMENT_BEHAVIOUR_H
#define _PLAYER_MOVEMENT_BEHAVIOUR_H

#include "Behaviour.h"

class Input;
class PlayerMovementBehaviour :
	public Behaviour
{
public:
	PlayerMovementBehaviour();
	~PlayerMovementBehaviour();

	bool Init(GameObject* newGameObject);
	bool CleanUp();

	bool Start();

	bool Update(float dt);

private:

	Input* input_;

	float move_acceleration_;
	float move_deceleration_;

	float max_vel_;
};

#endif