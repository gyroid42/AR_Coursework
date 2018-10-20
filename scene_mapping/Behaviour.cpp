#include "Behaviour.h"

#include "GameObject.h"


Behaviour::Behaviour() :
	gameobject_(nullptr)
{

}
Behaviour::~Behaviour() {

}


// Init
// Sets reference to object its attached to and gives it the base behaviour name
bool Behaviour::Init(GameObject* newGameObject) {

	behaviour_name_ = BASE_BEHAVIOUR;
	gameobject_ = newGameObject;

	return true;
}

// CleanUp Method
bool Behaviour::CleanUp() {

	// delete resources used here
	return true;
}


// Empty methods for initialising virtual methods

bool Behaviour::Awake() {

	return true;
}
bool Behaviour::Start() {

	return true;
}





// Update methods ////////

bool Behaviour::Update(float dt) {


	return true;
}

bool Behaviour::EarlyUpdate(float dt) {

	return true;
}

bool Behaviour::OnUpdateMesh() {

	return true;
}


// Render Methods //////////////


bool Behaviour::PreRenderGUI(gef::SpriteRenderer* sprite_renderer) {

	return true;
}


bool Behaviour::PreRender(gef::Renderer3D* renderer_3d) {

	return true;
}

bool Behaviour::OnRender(gef::Renderer3D* renderer_3d) {

	return true;
}

bool Behaviour::PostRender(gef::Renderer3D* renderer_3d) {

	return true;
}


bool Behaviour::OnGUI(gef::SpriteRenderer* sprite_renderer) {


	return true;
}




// Collision Methods ////////////

bool Behaviour::OnCollision(GameObject* other) {


	return true;
}


bool Behaviour::OnEnterCollision(GameObject* other) {

	return true;
}

bool Behaviour::OnExitCollision(GameObject* other) {

	return true;
}






// Enable and Disable events ////

bool Behaviour::OnEnable() {

	return true;
}


bool Behaviour::OnDisable() {

	return true;
}

bool Behaviour::IsEnableChanging() {

	return (enabled_ != enabledChangeTo_);
}


// OnDestroy called when gameobject is destroyed

bool Behaviour::OnDestroy() {


	return true;
}


bool Behaviour::OnApplicationQuit() {

	return true;
}





