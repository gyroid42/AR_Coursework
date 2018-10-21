#pragma once

#ifndef _PLAYER_SHOOT_BEHAVIOUR_H
#define _PLAYER_SHOOT_BEHAVIOUR_H


#include "Behaviour.h"
#include "Timer.h"

class Input;


class PlayerShootBehaviour :
	public Behaviour
{
public:
	PlayerShootBehaviour();
	~PlayerShootBehaviour();


	bool Init(GameObject* newGameObject);

	bool CleanUp();

	bool OnDestroy();

	bool Start();

	bool Update(float dt);



	bool SetProjectile(GameObject* new_projectile);

private:


	GameObject* projectile_;
	
	Input* input_;

	Timer shoot_timer_;


	float shoot_delay_;
	float shoot_vel_;

};


#endif