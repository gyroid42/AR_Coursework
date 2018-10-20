#pragma once


#ifndef _GAMEOBJECT_MANAGER_H
#define _GAMEOBJECT_MANAGER_H

#include <list>
#include <vector>
#include <string>


// Forward declarations
class GameObject;

namespace gef {
	class Renderer3D;
	class SpriteRenderer;
}



// Gameobject manager class
// Manages the gameobjects in the game
// Calls Update/Render/Destroy methods on game objects
class GameObjectManager
{
public:
	GameObjectManager();
	~GameObjectManager();


	// Initialisation and CleanUp methods
	bool Init();
	bool CleanUp();

	////////////////////////

	// Adds gameObject to manager
	GameObject* AddGameObject(GameObject* newObject);


	// Events for game objects //////////////

	// Update Methods //
	bool Update(float dt);
	bool EarlyUpdate(float dt);
	bool UpdateMeshes();


	bool CheckCollisions();

	// Render Methods //
	bool PreRenderGUI(gef::SpriteRenderer* sprite_renderer);
	bool OnGUI(gef::SpriteRenderer* sprite_renderer);
	bool Render(gef::Renderer3D* renderer_3d);

	// Destroys game Objects //
	bool DeleteObjects();



	bool ClearObjects();



	GameObject* FindObjectWithName(std::string name);


	// static reference to gameobject manager
	inline static GameObjectManager* GetGameObjectManager() { return gameobject_manager_; }

private:


	// Used to access gameobject manager in rest of application
	static GameObjectManager* gameobject_manager_;


	// List of gameobjects in manager
	std::list<GameObject*> gameObjects_;

	// List of gameobjects to be deleted at end of frame
	std::vector<std::list<GameObject*>::iterator> deleteList_;
	

};

#endif