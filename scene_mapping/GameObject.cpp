#include "GameObject.h"

#include "Behaviour.h"
#include <graphics/mesh_instance.h>
#include <maths\quaternion.h>
#include <graphics\renderer_3d.h>
#include <graphics\sprite_renderer.h>

GameObject::GameObject() :
	velocity_(gef::Vector4(0.0f, 0.0f, 0.0f)),
	position_(gef::Vector4(0.0f, 0.0f, 0.0f)),
	scale_(gef::Vector4(1.0f, 1.0f, 1.0f)),
	visible_(true),
	destroyed_(false),
	name_("")
{
	rotation_.Identity();
	//destroyed_ = false;
	//visible_ = true;
	//name_ = "";
}


GameObject::~GameObject()
{
}



Behaviour* GameObject::GetBehaviour(BEHAVIOUR_NAME name) {

	for (int i = 0; i < behaviours_.size(); i++) {

		if (behaviours_.at(i)->Name() == name) {

			return behaviours_.at(i);
		}
	}

	return nullptr;
}


void GameObject::AddBehaviour(Behaviour* new_behaviour) {

	new_behaviour->Init(this);
	new_behaviour->Awake();
	new_behaviour->Start();

	behaviours_.push_back(new_behaviour);
}

void GameObject::AddBehaviour(std::vector<Behaviour*> new_behaviours) {

	for (auto& iterator : new_behaviours) {

		AddBehaviour(iterator);
	}
}


// Updates mesh from game scale, rotation and position
void GameObject::UpdateMesh() {

	gef::Matrix44 scaleMatrix;
	gef::Matrix44 rotMatrix;

	rotMatrix.Rotation(rotation_);
	scaleMatrix.Scale(scale_);

	local_transform_ = scaleMatrix * rotMatrix;

	local_transform_.SetTranslation(position_);


	for (auto& iterator : behaviours_) {


		iterator->OnUpdateMesh();
	}


	set_transform(local_transform_);


	// loop through here and modify update mesh
}


// Init method
bool GameObject::Init() {

	destroyed_ = false;
	visible_ = true;
	return true;
}

// Update method called each frame
bool GameObject::Update(float dt) {


	for (auto& iterator : behaviours_) {

		iterator->Update(dt);
	}

	position_ += (velocity_ * dt);

	return true;
}

// Late update called before update each frame
bool GameObject::EarlyUpdate(float dt) {


	for (auto& iterator : behaviours_) {

		iterator->EarlyUpdate(dt);
	}
	return !destroyed_;
}


// Method called when destroyed
bool GameObject::OnDestroy() {

	for (auto& iterator : behaviours_) {

		iterator->OnDestroy();
	}
	return true;
}



// Render methods ///////////


bool GameObject::PreRenderGUI(gef::SpriteRenderer* sprite_renderer) {

	for (auto& iterator : behaviours_) {

		iterator->PreRenderGUI(sprite_renderer);
	}

	return true;
}


bool GameObject::PreRender(gef::Renderer3D* renderer_3d) {

	for (auto& iterator : behaviours_) {

		iterator->PreRender(renderer_3d);
	}

	return true;
}

bool GameObject::OnRender(gef::Renderer3D* renderer_3d) {

	if (mesh()) {

		renderer_3d->DrawMesh(*this);
	}

	for (auto& iterator : behaviours_) {

		iterator->OnRender(renderer_3d);
	}

	return true;
}


bool GameObject::PostRender(gef::Renderer3D* renderer_3d) {

	for (auto& iterator : behaviours_) {

		iterator->PostRender(renderer_3d);
	}

	return true;
}


bool GameObject::OnGUI(gef::SpriteRenderer* sprite_renderer) {

	for (auto& iterator : behaviours_) {

		iterator->OnGUI(sprite_renderer);
	}
	return true;
}




// Collision methods //////

bool GameObject::OnCollision(GameObject* other) {

	for (auto& iterator : behaviours_) {

		iterator->OnCollision(other);
	}

	return true;
}


bool GameObject::OnEnterCollision(GameObject* other) {

	for (auto& iterator : behaviours_) {

		iterator->OnEnterCollision(other);
	}

	return true;
}

bool GameObject::OnExitCollision(GameObject* other) {

	for (auto& iterator : behaviours_) {

		iterator->OnExitCollision(other);
	}

	return true;
}

