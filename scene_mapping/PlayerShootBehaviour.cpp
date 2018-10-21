

#include "PlayerShootBehaviour.h"

#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include <maths\Vector4.h>
#include <input\sony_controller_input_manager.h>
#include "GameObjectManager.h"


PlayerShootBehaviour::PlayerShootBehaviour() :
	Behaviour(),
	projectile_(nullptr),
	input_(nullptr)
{

}
PlayerShootBehaviour::~PlayerShootBehaviour() {

}




bool PlayerShootBehaviour::Init(GameObject* newGameObject) {

	Behaviour::Init(newGameObject);

	behaviour_name_ = PLAYER_SHOOT_BEHAVIOUR;

	return true;
}


bool PlayerShootBehaviour::CleanUp() {

	if (projectile_) {
		projectile_->OnDestroy();
		delete projectile_;
		projectile_ = nullptr;
	}

	return true;
}

bool PlayerShootBehaviour::OnDestroy() {

	return true;
}

bool PlayerShootBehaviour::Start() {

	input_ = Input::GetInput();

	shoot_vel_ = 1.0f;
	shoot_delay_ = 0.2f;

	shoot_timer_.SetTimer(shoot_delay_);

	return true;
}

bool PlayerShootBehaviour::Update(float dt) {

	shoot_timer_.Update(dt);

	gef::Vector4 forward = Camera::GetCamera()->GetForwardInput();
	gef::Vector4 right = Camera::GetCamera()->GetRightInput();

	const gef::SonyController* controller = input_->GetController();

	if (shoot_timer_.IsFinished()) {

		if (controller->right_stick_y_axis() != 0.0f || controller->right_stick_x_axis() != 0.0f) {

			gef::Vector4 shootDirection;
			shootDirection += right * controller->left_stick_x_axis();
			shootDirection += left * controller->left_stick_y_axis();

			shootDirection.Normalise();


			// spawn the projectile and give it the velocity in da direction bro


			
			Bullet* new_bullet = GameObjectManager::GetGameObjectManager()->AddGameObject(new Bullet());

			new_bullet->SetPosition(gameObject->Position());
			new_bullet->SetVelocity(shootDirection * shoot_vel_);

			new_bullet->GetBehaviour(BULLET_BEHAVIOUR)->SetOwner(OWNER_PLAYER);


			shoot_timer_.SetTimer(shoot_delay_);


		}
	}

	return true;
}


bool PlayerShootBehaviour::SetProjectile(GameObject* new_projectile) {


	projectile_ = new_projectile;

	return true;
}

