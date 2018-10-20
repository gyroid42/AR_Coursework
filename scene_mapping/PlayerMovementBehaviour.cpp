#include "PlayerMovementBehaviour.h"



#include "Input.h"
#include <input/sony_controller_input_manager.h>
#include "Camera.h"
#include "GameObject.h"
#include <maths\Vector4.h>
#include <maths\vector2.h>
#include <math.h>


PlayerMovementBehaviour::PlayerMovementBehaviour() :
	Behaviour(),
	input_(nullptr)
{
}


PlayerMovementBehaviour::~PlayerMovementBehaviour()
{
}



bool PlayerMovementBehaviour::Init(GameObject* newGameObject) {

	Behaviour::Init(newGameObject);

	behaviour_name_ = PLAYER_MOVEMENT_BEHAVIOUR;

	return true;
}


bool PlayerMovementBehaviour::CleanUp() {


	return true;
}


bool PlayerMovementBehaviour::Start() {


	input_ = Input::GetInput();

	move_acceleration_ = 0.5f;
	move_deceleration_ = -0.5f;
	max_vel_ = 0.1f;

	return true;
}

bool PlayerMovementBehaviour::Update(float dt) {


	// Determine forward direction for controls
	gef::Vector4 forward = Camera::GetCamera()->GetForwardInput();

	/*
	gef::Vector4 cam_pos = Camera::GetCamera()->Position();
	gef::Vector4 cam_lookat = Camera::GetCamera()->GetLookAt();
	
	forward = cam_lookat - cam_pos;
	forward.set_z(0.0f);
	forward.Normalise();

	gef::Vector4 right = gef::Vector4(0.0f, 0.0f, 1.0f).CrossProduct(forward);
	right.Normalise();

	*/
	gef::Vector4 right = Camera::GetCamera()->GetRightInput();
	
	gef::Vector4 vel = gameobject_->Velocity();
	gef::Vector4 movement(0.0f, 0.0f, 0.0f);

	const gef::SonyController* controller = input_->GetController();

	
	if (controller->left_stick_y_axis() != 0.0f || controller->left_stick_x_axis() != 0.0f) {

		gef::Vector2 left_stick(controller->left_stick_x_axis(), controller->left_stick_y_axis());

		movement += right * left_stick.x;// controller->left_stick_x_axis();
		movement += forward * left_stick.y;// controller->left_stick_y_axis();

		//movement.set_x(controller->left_stick_x_axis());
		//movement.set_y(controller->left_stick_y_axis());

		movement.Normalise();

	 

		
		

		// Move player based on joystick input and forward direction determined from camera
		/*
		vel += (movement * move_acceleration_ * dt);
		if (vel.LengthSqr() > max_vel_ * max_vel_) { // consider factoring in amount joystick is pushed in a direction
			vel.Normalise();
			vel *= max_vel_;
		}
		*/

		vel = movement * max_vel_ * left_stick.Length();
		gameobject_->SetVelocity(vel);
	}
	else if (vel.LengthSqr() != 0.0f) {
		gef::Vector4 velOpp = vel;
		velOpp.Normalise();
		vel += (velOpp * move_deceleration_ * dt);
		if (vel.LengthSqr() < (move_deceleration_* dt) * (move_deceleration_ * dt)) {
			vel = gef::Vector4(0.0f, 0.0f, 0.0f);
		}

		gameobject_->SetVelocity(vel);
	}
	

	return true;
}
