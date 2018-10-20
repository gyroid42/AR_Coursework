#include "GameObjectManager.h"

#include <graphics\renderer_3d.h>
#include <graphics\sprite_renderer.h>
#include "GameObject.h"
#include "Collision.h"

GameObjectManager::GameObjectManager()
{
}
GameObjectManager::~GameObjectManager()
{
}


// Set singleton to nullptr
GameObjectManager* GameObjectManager::gameobject_manager_ = nullptr;


// Init and Cleanup methods /////
bool GameObjectManager::Init() {


	// Create singleton
	if (!gameobject_manager_) {

		gameobject_manager_ = this;
	}
	else if (gameobject_manager_ != this) {

		return false;
	}
	return true;
}

bool GameObjectManager::CleanUp() {

	ClearObjects();

	gameobject_manager_ = nullptr;

	return true;
}





// Add gameobject to manager
GameObject* GameObjectManager::AddGameObject(GameObject* newObject) {

	newObject->Init();

	gameObjects_.push_back(newObject);

	return newObject;;
}




// Update methods //////


bool GameObjectManager::Update(float dt) {

	for (auto iterator = gameObjects_.begin(); iterator != gameObjects_.end(); iterator++) {

		if (!(*iterator)->Update(dt)) {

			//deleteList_.push_back(iterator);
		}

	}

	return true;
}


bool GameObjectManager::EarlyUpdate(float dt) {


	for (auto iterator = gameObjects_.begin(); iterator != gameObjects_.end(); iterator++) {

		if (!(*iterator)->EarlyUpdate(dt)) {

			deleteList_.push_back(iterator);
		}
	}

	return true;
}


bool GameObjectManager::UpdateMeshes() {

	for (auto& iterator : gameObjects_) {

		iterator->UpdateMesh();
	}

	return true;
}



// Render Methods /////////


bool GameObjectManager::PreRenderGUI(gef::SpriteRenderer* sprite_renderer) {

	for (auto& iterator : gameObjects_) {

		iterator->PreRenderGUI(sprite_renderer);
	}

	return true;
}


bool GameObjectManager::Render(gef::Renderer3D* renderer_3d) {

	for (auto& iterator : gameObjects_) {

		if (iterator->Visible()) {

			iterator->PreRender(renderer_3d);

			iterator->OnRender(renderer_3d);
			//renderer_3d->DrawMesh(*iterator);

			iterator->PostRender(renderer_3d);
		}
	}


	return true;
}


bool GameObjectManager::OnGUI(gef::SpriteRenderer* sprite_renderer) {

	for (auto& iterator : gameObjects_) {

		iterator->OnGUI(sprite_renderer);
	}

	return true;
}



// Checks delete list and destroys all objects on it

bool GameObjectManager::DeleteObjects() {


	for (int i = 0; i < deleteList_.size(); i++) {

		if (*deleteList_.at(i)) {

			(*deleteList_.at(i))->OnDestroy();

			delete (*deleteList_.at(i));
			(*deleteList_.at(i)) = nullptr;

			gameObjects_.erase(deleteList_.at(i));
		}
	}

	deleteList_.clear();

	return true;
}




bool GameObjectManager::CheckCollisions() {


	for (auto i = gameObjects_.begin(); i != gameObjects_.end(); i++) {

		for (auto j = gameObjects_.end(); j != gameObjects_.end(); j++) {

			if (i != j) {
				if (Collision::SphereToSphere((*i), (*j))) {

					(*i)->OnCollision(*j);
					(*j)->OnCollision(*i);
				}
			}
		}
	}

	return true;
}






bool GameObjectManager::ClearObjects() {

	for (auto& iterator : gameObjects_) {

		if (iterator) {

			iterator->OnDestroy();

			delete iterator;
			iterator = nullptr;
		}
	}


	gameObjects_.clear();

	return true;
}






GameObject* GameObjectManager::FindObjectWithName(std::string name) {

	for (auto& iterator : gameObjects_) {

		if (iterator) {

			if (iterator->Name() == name) {

				return iterator;
			}
		}
	}

	return nullptr;
}
