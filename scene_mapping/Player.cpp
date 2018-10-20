#include "Player.h"


#include "BehaviourHeaders.h"
#include "MeshManager.h"


Player::Player() :
	GameObject()
{
}


Player::~Player()
{
}



bool Player::Init() {

	GameObject::Init();

	SetName("Player");


	AddBehaviour(new PlayerMovementBehaviour());
	AddBehaviour(new PlayerShootBehaviour());


	SetScale(gef::Vector4(0.2f, 0.2f, 0.2f));
	set_mesh(MeshManager::GetMeshManager()->GetMesh(BALL2));


	return true;
}


